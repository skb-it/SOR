#include "common.h"
#include "errors.h"
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t terminate = 0;

void handle_terminate(int sig) {
    (void)sig;
    terminate = 1;
}

void asses_doc(struct PatientCard *card){
    int random = rand() % 100;

    if(card->age < 18){
        if(random < 16) card->sdoc = DOC_PEDIATRICIAN;
        else if(random < 32) card->sdoc = DOC_EYE_DOC;
        else if(random < 60) card->sdoc = DOC_LARYNGOLOGIST;
        else if(random < 70) card->sdoc = DOC_NEUROLOGIST;
        else if(random < 80) card->sdoc = DOC_CARDIOLOGIST;
        else card->sdoc = DOC_SURGEON;
    }
    else {
        if(random < 20) card->sdoc = DOC_CARDIOLOGIST;
        else if(random < 40) card->sdoc = DOC_EYE_DOC;
        else if(random < 60) card->sdoc = DOC_LARYNGOLOGIST;
        else if(random < 80) card->sdoc = DOC_NEUROLOGIST;
        else card->sdoc = DOC_SURGEON;
    }
}

void triage(struct PatientCard *card){
    LOG_PRINTF("|DOCTOR %d| Examining Patient %d...", getpid(), card->patient_id);

    int random = rand() % 100;
    
    if (random < 10) {
        card->triage = TRIAGE_RED;
    } 
    else if (random < 45) {
        card->triage = TRIAGE_YELLOW;
    }   
    else if (random < 95) {
        card->triage = TRIAGE_GREEN;
    } 
    else {
        card->triage = SENT_HOME;
    }
}

int main(){
    struct sigaction sa;
    sa.sa_handler = handle_terminate;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    
    srand(time(NULL) ^ getpid());

    // SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[pc_doctor.c] key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, SHM_SIZE_CARD, 0600);
    if(shmget_reg_doc == -1) report_error("[pc_doctor.c] shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[pc_doctor.c] shmat card", 1);

    // MESSAGE QUEUE PC DOCTOR->PATIENT
    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[pc_doctor.c] key_msg_doc_pat", 1);
    
    int msg_doc_pat = msgget(key_msg_doc_pat, 0600);
    if(msg_doc_pat == -1) report_error("[pc_doctor.c] msg_doc_pat", 1);

    // SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600);
    if(semget_doc == -1) report_error("[pc_doctor.c] semget_doc", 1);

    LOG_PRINTF("|DOCTOR %d| Ready to receive patients.", getpid());

    while(terminate != 1){
        LOG_PRINTF("|DOCTOR %d| Waiting for a patient card...", getpid());

        int wait_result = consumer_wait_data(semget_doc, &terminate);
        if(wait_result != 0) {
            break;
        }

        struct PatientCard local_card = *card;

        LOG_PRINTF("|DOCTOR %d| Received Patient %d card!", getpid(), local_card.patient_id);

        if(consumer_signal_slot(semget_doc) == -1) {
            report_error("[pc_doctor.c] consumer_signal_slot", 0);
        }

        triage(&local_card);
        
        local_card.mtype = local_card.patient_id;
        
        if(local_card.triage == SENT_HOME){
            if(msgsnd(msg_doc_pat, &local_card, sizeof(struct PatientCard) - sizeof(long), 0) == -1) {
                if(errno != EINTR) {
                    report_error("[pc_doctor.c] msgsnd_doc_pat (home)", 1);
                }
            }
            increment_sent_home_count();
            LOG_PRINTF("|DOCTOR %d| Patient %d sent home.", getpid(), local_card.patient_id);
        }
        else{
            asses_doc(&local_card);
            
            if(msgsnd(msg_doc_pat, &local_card, sizeof(struct PatientCard) - sizeof(long), 0) == -1) {
                if(errno != EINTR) {
                    report_error("[pc_doctor.c] msgsnd_doc_pat (specialist)", 1);
                }
            }
            LOG_PRINTF("|DOCTOR %d| Patient %d sent to specialist %d.", getpid(), local_card.patient_id, local_card.sdoc);
        }

        increment_pc_doctor_count();
    }

    if(shmdt(card) == -1) {
        report_error("[pc_doctor.c] shmdt_reg_doc", 0);
    }

    LOG_PRINTF("|DOCTOR %d| Shutting down.", getpid());
    return 0;
}