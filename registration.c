#include "common.h"
#include "errors.h"

int main(){
    printf("|REGISTRATION %d| Opening...\n", getpid());
    key_t key_shm_pat_reg = ftok(FTOK_PATH, ID_SHM_PAT_REG);
    if(key_shm_pat_reg == -1){
        report_error("[registration.c] error: key_shm_pat_reg", 1);
    }

    struct Message msg_pat_reg;
     size_t size_pat_reg = sizeof(msg_pat_reg) - sizeof(long);

    int shm_id_pat_reg = shmget(key_shm_pat_reg, size_pat_reg, 0600 | IPC_CREAT);
    if(shm_id_pat_reg == -1){
        report_error("[registration.c] error: shm_id_pat_reg", 1);
    }
    
    void *shm_attach = shmat(shm_id_pat_reg, 0, 0);
    if(shm_attach == (void *) -1){
        report_error("[registration.c] error: shm_attach", 1);
    }

    struct Message *shared_message = (struct Message *)shm_attach;

    key_t key_msg_reg_doc = ftok(FTOK_PATH, ID_MSG_REG_DOC);
    if(key_msg_reg_doc == -1){
        report_error("[registration.c] error: key_msg_reg_doc", 1);
    }

    int msg_id_reg_doc = msgget(key_msg_reg_doc, 0600 | IPC_CREAT);
    if(msg_id_reg_doc == -1){
        report_error("[registration.c] error: msg_id_reg_doc", 1);
    }

    printf("|REGISTRATION %d| Opened!\n", getpid());

    while(1){
        

        printf("|REGISTRATION| Patient %d came!\n", msg_pat_reg.patient_id);

        struct Message msg_reg_doc;
        msg_reg_doc.mtype = msg_pat_reg.mtype;
        msg_reg_doc.patient_id = msg_pat_reg.patient_id;
        msg_reg_doc.age = msg_pat_reg.age;
        msg_reg_doc.is_vip = msg_pat_reg.is_vip;
        

        size_t size_reg_doc = sizeof(msg_reg_doc) - sizeof(long);
        int msg_send_reg_doc = msgsnd(msg_id_reg_doc, &msg_reg_doc, size_reg_doc, 0);
        if(msg_send_reg_doc == -1){
            report_error("[registration.c] error: msg_send_reg_doc", 1);
        }
        sleep(5);
        printf("|REGISTRATION| Patient %d forwarded to doctor.\n", msg_pat_reg.patient_id);
    }

    int shm_detach = shmdt(shm_attach);
    int shm_delete = shmctl(shm_id_pat_reg, IPC_RMID, NULL);
    
    return 0;
}
