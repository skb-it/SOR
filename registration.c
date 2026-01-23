#include "common.h"
#include "errors.h"


volatile int reg_close = 0;

void handle_close(int sig){
    reg_close = 1;
}

void free_queue_place(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    while(semop(semget, &sb, 1) == -1) {
        if(errno == EINTR) {
            if(reg_close) {
                return;
            }
            continue;
        }
        report_error("[registration.c] semop_give_info", 1);
    }
}


int main(){

    signal(SIGUSR2, handle_close);

    printf("|REGISTRATION %d| Opening...\n", getpid());
    
    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[registration.c] key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[registration.c] msg_pat_reg", 1);

    struct MsgBuffer buf;

    //SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[registration.c] key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard), 0600 | IPC_CREAT);
    if(shmget_reg_doc == -1) report_error("[registration.c] shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[registration.c] shmat card", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[registration.c] key_sem_msg_pat_reg", 1);

    int semget_msg_pat_reg = semget(key_sem_msg_pat_reg, 0, 0600);
    if(semget_msg_pat_reg == -1) report_error("[registration.c] semget_msg_pat_reg", 1);



    //SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[registration.c] key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600);
    if(semget_doc == -1) report_error("[registration.c] semget_doc", 1);

    struct sembuf wait_empty;
    wait_empty.sem_num = 0;
    wait_empty.sem_op = -1;
    wait_empty.sem_flg = SEM_UNDO;

    struct sembuf data_ready;
    data_ready.sem_num = 1;
    data_ready.sem_op = 1;
    data_ready.sem_flg = SEM_UNDO;

    printf("|REGISTRATION %d| Opened!\n", getpid());

    while(1){
        if (reg_close == 1){
            printf("|REGISTRATION %d| Closing registration...\n", getpid());
            break;
        }

        int msgrcv_pat_reg = msgrcv(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), -3, 0);
        if(msgrcv_pat_reg == -1) report_error("[registration.c] msgrcv_pat_reg", 1);

        if(msgrcv_pat_reg == -1) {
            if (errno == EINTR) {
                if(reg_close == 1) {
                    printf("|REGISTRATION %d| Closing registration...\n", getpid());
                    break;
                }
                continue;
            }
            report_error("[registration.c] msgrcv_pat_reg", 1);
        }

        printf("|REGISTRATION %d| Patient %d came!\n", getpid(), buf.patient_id);
        //sleep(2);

        printf("|REGISTRATION %d| Waiting for free doctor slot...\n", getpid());

        while(semop(semget_doc, &wait_empty, 1) == -1) {
             if(errno == EINTR) {
                if(reg_close == 1){
                    break;
                }
                continue;
             }
             report_error("[registration.c] semop_wait_empty", 1);
        }
        
        if(reg_close) {
             printf("|REGISTRATION %d| Closing registration (interrupted)...\n", getpid());
             break;
        }

        card->age = buf.age;
        card->patient_id = buf.patient_id;
        card->is_guardian = buf.is_guardian;
        card->is_vip = (buf.mtype == VIP) ? 1 : 0;

        printf("|REGISTRATION %d| Patient %d forwarded to primary care doctor!", getpid(), card->patient_id);

        while(semop(semget_doc, &data_ready, 1) == -1) {
             if(errno != EINTR) report_error("[registration.c] semop_data_ready", 1);
        }

        if(reg_close == 1){
            break;
        }

        free_queue_place(semget_msg_pat_reg);
    }
    

    //DETACHING SHARED MEMORY REGISTRATION->PC_DOCTOR
    int shmdt_card = shmdt(card);
    if(shmdt_card== -1) report_error("[registration.c] shmdt_card", 1);
    

    return 0;
}