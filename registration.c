#include "common.h"
#include "errors.h"

volatile sig_atomic_t reg_close = 0;
volatile sig_atomic_t terminate = 0;

void handle_close(int sig){
    (void)sig;
    reg_close = 1;
}

void handle_terminate(int sig) {
    (void)sig;
    terminate = 1;
    reg_close = 1;
}

int main(){
    struct sigaction sa_term, sa_close;
    
    sa_term.sa_handler = handle_terminate;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);
    
    sa_close.sa_handler = handle_close;
    sigemptyset(&sa_close.sa_mask);
    sa_close.sa_flags = 0;
    sigaction(SIGUSR2, &sa_close, NULL);

    LOG_PRINTF("|REGISTRATION %d| Opening...", getpid());
    
    // MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[registration.c] key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600);
    if(msg_pat_reg == -1) report_error("[registration.c] msg_pat_reg", 1);

    struct Msg buf;

    // SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[registration.c] key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard), 0600);
    if(shmget_reg_doc == -1) report_error("[registration.c] shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[registration.c] shmat card", 1);

    // SEMAPHORE MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[registration.c] key_sem_msg_pat_reg", 1);

    int semget_msg_pat_reg = semget(key_sem_msg_pat_reg, 1, 0600);
    if(semget_msg_pat_reg == -1) report_error("[registration.c] semget_msg_pat_reg", 1);

    // SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[registration.c] key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600);
    if(semget_doc == -1) report_error("[registration.c] semget_doc", 1);

    LOG_PRINTF("|REGISTRATION %d| Opened!", getpid());

    while(terminate != 1 && !reg_close) {
        int msgrcv_pat_reg = msgrcv(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), -3, 0);
        
        if(msgrcv_pat_reg == -1) {
            if(errno == EINTR) {
                continue;
            }
            report_error("[registration.c] msgrcv_pat_reg", 1);
        }

        sem_release(semget_msg_pat_reg);

        LOG_PRINTF("|REGISTRATION %d| Patient %d came!", getpid(), buf.patient_id);

        int wait_result = producer_wait_slot(semget_doc, &terminate);
        if(wait_result != 0) {
            LOG_PRINTF("|REGISTRATION %d| Interrupted while waiting for doctor slot", getpid());
            break;
        }

        card->mtype = 0;
        card->age = buf.age;
        card->patient_id = buf.patient_id;
        card->is_guardian = buf.is_guardian;
        card->is_vip = (buf.mtype == VIP) ? 1 : 0;
        card->triage = 0;
        card->sdoc = 0;
        card->sdoc_dec = 0;
        card->flag = 0;

        LOG_PRINTF("|REGISTRATION %d| Patient %d forwarded to primary care doctor!", getpid(), card->patient_id);

        if(producer_signal_data(semget_doc) == -1) {
            report_error("[registration.c] producer_signal_data", 0);
        }
    }

    if(shmdt(card) == -1) {
        report_error("[registration.c] shmdt_card", 0);
    }

    LOG_PRINTF("|REGISTRATION %d| Closed.", getpid());
    return 0;
}