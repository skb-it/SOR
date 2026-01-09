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

    printf("|REGISTRATION %d| Opened!\n", getpid());

    while(1){
        int msgrcv_pat_reg = msgrcv(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), -1, 0);
        printf("|REGISTRATION %d| Patient %d came!\n", getpid(), buf.patient_id);
        printf("Age: %d, Guardian: ", buf.age);
        sleep(5);

        card->age = buf.age;
        card->patient_id = buf.patient_id;
        card->is_guardian = buf.is_guardian;
        card->mtype;

        printf("|REGISTRATION %d| Patient %d forwarded to primary care doctor!", getpid(), card->patient_id);
    }
    

    //DETACHING SHARED MEMORY (PATIENT->REGISTRATION)
    int shmdt_info = shmdt(&buf);
    if(shmdt_info == -1) report_error("[patient.c] error: shmdt_info", 1);


    return 0;
}