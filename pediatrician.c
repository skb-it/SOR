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
    LOG_PRINTF("|PEDIATRICIAN %d| Going to ward...", getpid());
    LOG_PRINTF("|PEDIATRICIAN %d| Returned from ward.", getpid());
    go_to_ward = 0;
}

int main(){
    struct sigaction sa_term, sa_ward;
    
    sa_term.sa_handler = handle_terminate;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if(sigaction(SIGTERM, &sa_term, NULL) == -1) report_error("[pediatrician.c] sigaction SIGTERM", 1);
    
    sa_ward.sa_handler = handle_signal;
    sigemptyset(&sa_ward.sa_mask);
    sa_ward.sa_flags = 0;
    if(sigaction(SIGUSR1, &sa_ward, NULL) == -1) report_error("[pediatrician.c] sigaction SIGUSR1", 1);

    srand(time(NULL) ^ getpid());
    
    // MESSAGE QUEUE PATIENT<->PEDIATRICIAN
    key_t key_msg = ftok(FTOK_PATH, ID_MSG_PAT_PEDIATR);
    if(key_msg == -1) report_error("[pediatrician.c] ftok msg", 1);

    int msg_queue = msgget(key_msg, 0600);
    if(msg_queue == -1) report_error("[pediatrician.c] msgget", 1);

    // SEMAPHORE FOR QUEUE
    key_t key_sem = ftok(FTOK_PATH, ID_SEM_MSG_PEDIATR);
    if(key_sem == -1) report_error("[pediatrician.c] ftok sem", 1);

    int sem_pediatr = semget(key_sem, 1, 0600);
    if(sem_pediatr == -1) report_error("[pediatrician.c] sem_pediatr", 1);

    LOG_PRINTF("|PEDIATRICIAN %d| Ready.", getpid());

    struct PatientCard card;

    while(!terminate){
        if(go_to_ward){
            visit_ward();
        }

        LOG_PRINTF("|PEDIATRICIAN %d| Waiting for patient...", getpid());

        int msgrcv_pat_pediatr = msgrcv(msg_queue, &card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if(msgrcv_pat_pediatr == -1) {
            if(errno == EINTR) {
                continue;
            }
            report_error("[pediatrician.c] msgrcv", 1);
        }

        LOG_PRINTF("|PEDIATRICIAN %d| Examining patient %d...", getpid(), card.patient_id);

        int random = rand() % 1000;
        if(random < 145) {
            card.sdoc_dec = SENT_TO_WARD;
        } else if(random < 150) {
            card.sdoc_dec = OTHER_S_HOSP;
        } else {
            card.sdoc_dec = SENT_HOME;
        }

        card.mtype = card.patient_id;

        while(msgsnd(msg_queue, &card, sizeof(struct PatientCard) - sizeof(long), 0) == -1) {
            if(errno == EINTR) {
                if(go_to_ward) visit_ward();
                continue;
            }
            report_error("[pediatrician.c] msgsnd", 1);
        }

        LOG_PRINTF("|PEDIATRICIAN %d| Patient %d examinated!", getpid(), card.patient_id);

        sem_release(sem_pediatr);
        increment_doctor_count(DOC_PEDIATRICIAN);
    }

    LOG_PRINTF("|PEDIATRICIAN %d| Evacuation!", getpid());
    return 0;
}