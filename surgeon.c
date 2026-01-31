#include "errors.h"
#include "common.h"

volatile sig_atomic_t go_to_ward = 0;
volatile sig_atomic_t terminate = 0;

void handle_signal(int sig){
    (void)sig;
    go_to_ward = 1;
}

void handle_terminate(int sig) {
    (void)sig;
    terminate = 1;
}

void visit_ward(){
    LOG_PRINTF("|SURGEON %d| Going to ward...", getpid());
    LOG_PRINTF("|SURGEON %d| Returned from ward.", getpid());
    go_to_ward = 0;
}

int main(){
    struct sigaction sa_term, sa_ward;
    
    sa_term.sa_handler = handle_terminate;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if(sigaction(SIGTERM, &sa_term, NULL) == -1) report_error("[surgeon.c] sigaction SIGTERM", 1);
    
    sa_ward.sa_handler = handle_signal;
    sigemptyset(&sa_ward.sa_mask);
    sa_ward.sa_flags = 0;
    if(sigaction(SIGUSR1, &sa_ward, NULL) == -1) report_error("[surgeon.c] sigaction SIGUSR1", 1);

    srand(time(NULL) ^ getpid());
    
    // MESSAGE QUEUE PATIENT<->SURGEON
    key_t key_msg_pat_surgeon = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon == -1) report_error("[surgeon.c] key_msg_pat_surgeon", 1);

    int msgget_pat_surgeon = msgget(key_msg_pat_surgeon, 0600);
    if(msgget_pat_surgeon == -1) report_error("[surgeon.c] msgget", 1);

    // SEMAPHORE FOR QUEUE
    key_t key_sem_surgeon = ftok(FTOK_PATH, ID_SEM_MSG_SURGEON);
    if(key_sem_surgeon == -1) report_error("[surgeon.c] key_sem_surgeon", 1);

    int sem_surgeon = semget(key_sem_surgeon, 1, 0600);
    if(sem_surgeon == -1) report_error("[surgeon.c] sem_surgeon", 1);

    LOG_PRINTF("|SURGEON %d| Ready.", getpid());

    struct PatientCard card;

    while(terminate!=1){
        if(go_to_ward==1){
            visit_ward();
        }

        LOG_PRINTF("|SURGEON %d| Waiting for patient...", getpid());

        int msgrcv_pat_surgeon = msgrcv(msgget_pat_surgeon, &card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if(msgrcv_pat_surgeon == -1) {
            if(errno == EINTR) {
                continue;
            }
            report_error("[surgeon.c] msgrcv_pat_surgeon", 1);
        }

        LOG_PRINTF("|SURGEON %d| Examining patient %d...", getpid(), card.patient_id);

        int random = rand() % 1000;
        if(random < 145) {
            card.sdoc_dec = SENT_TO_WARD;
        } else if(random < 150) {
            card.sdoc_dec = OTHER_S_HOSP;
        } else {
            card.sdoc_dec = SENT_HOME;
        }

        card.mtype = card.patient_id;
        int msgsnd_surgeon_pat = msgsnd(msgget_pat_surgeon, &card, sizeof(struct PatientCard) - sizeof(long), 0);
        while(msgsnd_surgeon_pat == -1){
            if(errno == EINTR) {
                if(go_to_ward) visit_ward();
                continue;
            }
            report_error("[surgeon.c] msgsnd", 1);
        }

        LOG_PRINTF("|SURGEON %d| Patient %d examinated", getpid(), card.patient_id);

        sem_release(sem_surgeon);
        increment_doctor_count(DOC_SURGEON);
    }

    LOG_PRINTF("|SURGEON %d| Evacuation!", getpid());
    return 0;
}