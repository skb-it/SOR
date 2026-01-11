#include "common.h"
#include "errors.h"

void triage(struct PatientCard *card){
    printf("|DOCTOR %d| Waiting for a patient...\n", getpid());

    //SEMAPHORE - WAITING UNTIL REGISTRATION WILL SEND THE DATA

    sleep(5);
    printf("|DOCTOR %d| Patient %d came!\n", getpid(), card->patient_id);
    sleep(1);
    printf("|DOCTOR %d| Examining Patient %d)", getpid(), card->patient_id);
    sleep(5);
    printf("|DOCTOR %d| Assesing triage for Patient %d...", getpid(), card->patient_id);
    sleep(1);
    triage(card);
    printf("|DOCTOR %d| Triage assesed for Patient %d.\n", getpid(),card->patient_id);
    sleep(1);
    int random = rand() % 100;
    
    if (random < 10) {
        card->triage = TRIAGE_RED;
        printf("|DOCTOR %d| PATIENT %d -> RED TRIAGE\n", getpid(), card->patient_id);
    } else if (random < 40) {
        card->triage = TRIAGE_YELLOW;
        printf("|DOCTOR %d| PATIENT %d -> YELLOW TRIAGE\n", getpid(), card->patient_id);
    } else if (random < 80) {
        card->triage = TRIAGE_GREEN;
        printf("|DOCTOR %d| PATIENT %d -> GREEN) TRIAGE\n", getpid(), card->patient_id);
    } else {
        card->triage = SENT_HOME;
        printf("|DOCTOR %d| PATIENT %d -> SENT TO HOME\n", getpid(), card->patient_id);
    }
}

int main(){
    

    //SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[pc_doctor.c] error: key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, SHM_SIZE_CARD, 0600 | IPC_CREAT);
    if(shmget_reg_doc == -1) report_error("[pc_doctor.c] error: shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[pc_doctor.c] error: shmat (reg->doc)", 1);

    //SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 1, 0600 | IPC_CREAT);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    union semun arg;
    arg.val = 3;
    int semctl_doc = semctl(semget_doc, 0 , SETVAL, arg);
    if(semctl_doc == -1) report_error("[pc_doctor.c] error: semtcl_doc", 1);


    while(1){
        triage(card);
        sleep(1);

        //SEMAPHORE - WAITING FOR NEW PATIENT
    }

    int shmdt_reg_doc = shmdt(card);
    if(shmdt_reg_doc == -1) report_error("[pc_doctor.c] error: shmdt_reg_doc", 1);

    return 0;
}