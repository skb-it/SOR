#include "common.h"
#include "errors.h"


int main(){
    printf("|REGISTRATION %d| Opening...\n", getpid());
    
    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[registration.c] error: key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[registration.c] error: msg_pat_reg", 1);

    struct MsgBuffer buf;

    //SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[registration.c] error: key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, SHM_SIZE_CARD, 0600 | IPC_CREAT);
    if(shmget_reg_doc == -1) report_error("[registration.c] error: shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[registration.c] error: shmat (reg->doc)", 1);

    //SEMAPHORE REGISTRATION
    key_t key_sem_reg = ftok(FTOK_PATH, ID_SEM_REG);
    if(key_sem_reg == -1) report_error("[patient.c] error: key_sem_rem", 1);

    int semget_reg = semget(key_sem_reg, 1, 0600 | IPC_CREAT);
    if(semget_reg == -1) report_error("[patient.c] error: semget_reg", 1);

    int semctl_reg = semctl(semget_reg, 0 , SETVAL, 1);
    if(semctl_reg == -1) report_error("[patient.c] error: semtcl_reg", 1);


    //SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);\

    struct sembuf wait_empty;
    wait_empty.sem_num = 0;
    wait_empty.sem_op = -1;
    wait_empty.sem_flg = SEM_UNDO;

    struct sembuf signal_data_ready;
    signal_data_ready.sem_num = 1;
    signal_data_ready.sem_op = 1;
    signal_data_ready.sem_flg = SEM_UNDO;

    printf("|REGISTRATION %d| Opened!\n", getpid());

    while(1){
        int msgrcv_pat_reg = msgrcv(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), -1, 0);
        if(msgrcv_pat_reg == -1) report_error("[patient.c] error: msgrcv_pat_reg", 1);

        printf("|REGISTRATION %d| Patient %d came!\n", getpid(), buf.patient_id);
        sleep(2);

        printf("|REGISTRATION %d| Waiting for free doctor slot...\n", getpid());
        int semop_wait_empty = semop(semget_doc, &wait_empty, 1);
        if (semop_wait_empty == -1) report_error("[registration.c] error: semop_wait_empty", 1);


        card->age = buf.age;
        card->patient_id = buf.patient_id;
        card->is_guardian = buf.is_guardian;
        card->mtype = buf.mtype;

        printf("|REGISTRATION %d| Patient %d forwarded to primary care doctor!", getpid(), card->patient_id);

        int semop_signal_data_ready = semop(semget_doc, &signal_data_ready, 1);
        if(semop_signal_data_ready == -1) report_error("[registration.c] error: semop_signal_data_ready", 1);

        
    }
    

    //DETACHING SHARED MEMORY REGISTRATION->PC_DOCTOR
    int shmdt_card = shmdt(card);
    if(shmdt_card== -1) report_error("[registration.c] error: shmdt_card", 1);
    

    //DELETING REGISTRATION SEMAPHORE
    int semctl_del_reg = semctl(semget_reg, 0 , IPC_RMID, 1);
    if(semctl_del_reg == -1) report_error("[registration.c] error: semtcl_del_reg", 1);

    printf("|REGISTRATION %d| Closed!\n", getpid());
    return 0;
}