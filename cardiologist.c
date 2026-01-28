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
    LOG_PRINTF("|CARDIOLOGIST %d| Going to ward...", getpid());
    //int _time = rand() % 100;
    //sleep(random_time) 
    LOG_PRINTF("|CARDIOLOGIST %d| Returned from ward.", getpid());
    go_to_ward = 0;
}

int main(){
    struct sigaction sa_term, sa_ward;
    
    sa_term.sa_handler = handle_terminate;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);
    
    sa_ward.sa_handler = handle_signal;
    sigemptyset(&sa_ward.sa_mask);
    sa_ward.sa_flags = 0;
    sigaction(SIGUSR1, &sa_ward, NULL);

    srand(time(NULL) ^ getpid());
    
    // MESSAGE QUEUE PATIENT<->CARDIOLOGIST
    key_t key_msg_pat_cardio = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio == -1) report_error("[cardiologist.c] key_msg_pat_cardio", 1);

    int msgget_pat_cardio = msgget(key_msg_pat_cardio, 0600);
    if(msgget_pat_cardio == -1) report_error("[cardiologist.c] msgget_pat_cardio", 1);

    // SEMAPHORE FOR QUEUE
    key_t key_sem_msg_cardio = ftok(FTOK_PATH, ID_SEM_MSG_CARDIO);
    if(key_sem_msg_cardio == -1) report_error("[cardiologist.c] key_sem_msg_cardio", 1);

    int semget_msg_cardio = semget(key_sem_msg_cardio, 1, 0600);
    if(key_sem_msg_cardio == -1) report_error("[cardiologist.c] key_sem_msg_cardio", 1);

    LOG_PRINTF("|CARDIOLOGIST %d| On duty.", getpid());

    struct PatientCard card;

    while(terminate == 0){
        if(go_to_ward == 1){
            visit_ward();
        }

        LOG_PRINTF("|CARDIOLOGIST %d| Waiting for patient...", getpid());

        int msgrcv_pat_cardio = msgrcv(msgget_pat_cardio, &card, sizeof(struct PatientCard) - sizeof(long), -3, 0);
        if(msgrcv_pat_cardio == -1) {
            if(errno == EINTR) {
                continue;
            }
            report_error("[cardiologist.c] msgrcv_pat_cardio", 1);
        }

        LOG_PRINTF("|CARDIOLOGIST %d| Examining patient %d...", getpid(), card.patient_id);

        int random = rand() % 1000;
        if(random < 145) {
            card.sdoc_dec = SENT_TO_WARD;
        } else if(random < 150) {
            card.sdoc_dec = OTHER_S_HOSP;
        } else {
            card.sdoc_dec = SENT_HOME;
        }

        card.mtype = card.patient_id;

        int msgsnd_cardio_pat = msgsnd(msgget_pat_cardio, &card, sizeof(struct PatientCard) - sizeof(long), 0);

        while(msgsnd_cardio_pat == -1) {
            if(errno == EINTR) {
                if(go_to_ward) visit_ward();
                continue;
            }
            report_error("[cardiologist.c] msgsnd_cardio_pat", 1);
        }

        LOG_PRINTF("|CARDIOLOGIST %d| Patient %d is examinated", getpid(), card.patient_id);

        sem_release(semget_msg_cardio);
        
        increment_doctor_count(DOC_CARDIOLOGIST);
    }

    LOG_PRINTF("|CARDIOLOGIST %d| Evacuation!", getpid());
    return 0;
}