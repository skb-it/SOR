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
    LOG_PRINTF("|NEUROLOGIST %d| Going to ward...", getpid());

    //int _time = rand() % 100;
    //sleep(random_time) 

    LOG_PRINTF("|NEUROLOGIST %d| Returned from ward.", getpid());
    go_to_ward = 0;
}

int main(){
    struct sigaction sa_term, sa_ward;
    
    sa_term.sa_handler = handle_terminate;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if(sigaction(SIGTERM, &sa_term, NULL) == -1) report_error("[neurologist.c] sigaction SIGTERM", 1);
    
    sa_ward.sa_handler = handle_signal;
    sigemptyset(&sa_ward.sa_mask);
    sa_ward.sa_flags = 0;
    if(sigaction(SIGUSR1, &sa_ward, NULL) == -1) report_error("[neurologist.c] sigaction SIGUSR1", 1);

    srand(time(NULL) ^ getpid());
    
    // MESSAGE QUEUE PATIENT<->NEUROLOGIST
    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[neurologist.c] key_msg_pat_neuro", 1);

    int msgget_pat_neuro = msgget(key_msg_pat_neuro, 0600);
    if(msgget_pat_neuro == -1) report_error("[neurologist.c] msgget_pat_neuro", 1);

    // SEMAPHORE FOR QUEUE
    key_t key_sem_neuro = ftok(FTOK_PATH, ID_SEM_MSG_NEURO);
    if(key_sem_neuro == -1) report_error("[neurologist.c] key_sem_neuro", 1);

    int semget_neuro = semget(key_sem_neuro, 1, 0600);
    if(semget_neuro == -1) report_error("[neurologist.c] semget_neuro", 1);

    struct PatientCard card;

    LOG_PRINTF("|NEUROLOGIST %d| Ready.", getpid());

    while(terminate!=1){
        if(go_to_ward==1){
            visit_ward();
        }

        LOG_PRINTF("|NEUROLOGIST %d| Waiting for patient...", getpid());

        int msgrcv_pat_neuro = msgrcv(msgget_pat_neuro, &card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if(msgrcv_pat_neuro == -1) {
            if(errno == EINTR) {
                continue;
            }
            report_error("[neurologist.c] msgrcv_pat_neuro", 1);
        }

        LOG_PRINTF("|NEUROLOGIST %d| Examining patient %d...", getpid(), card.patient_id);

        int random = rand() % 1000;
        if(random < 145) {
            card.sdoc_dec = SENT_TO_WARD;
        } else if(random < 150) {
            card.sdoc_dec = OTHER_S_HOSP;
        } else {
            card.sdoc_dec = SENT_HOME;
        }

        card.mtype = card.patient_id;

        int msgsnd_neuro_pat = msgsnd(msgget_pat_neuro, &card, sizeof(struct PatientCard) - sizeof(long), 0);
        while(msgsnd_neuro_pat == -1) {
            if(errno == EINTR) {
                if(go_to_ward) visit_ward();
                continue;
            }
            report_error("[neurologist.c] msgsnd_neuro_pat", 1);
        }

        LOG_PRINTF("|NEUROLOGIST %d| Patient %d examinated!", getpid(), card.patient_id);

        sem_release(semget_neuro);
        increment_doctor_count(DOC_NEUROLOGIST);
    }

    LOG_PRINTF("|NEUROLOGIST %d| Evacuation!", getpid());
    return 0;
}