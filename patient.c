#include "common.h"

int main(){
    printf("|PATIENT %d| Creating...\n", getpid());

    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    int msg_id_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if( msg_id_pat_reg == -1){
        perror("[patient.c] error: msg_id_pat_reg");
        exit(1);
    }

    

    while(1){
        struct Message msg_pat;
        msg_pat.patient_id = getpid();
        msg_pat.age = rand() % (116 + 1 - 0) + 0;
        if (rand() % 100 < 5) {
            msg_pat.mtype = 1;
            msg_pat.is_vip = VIP;
        } 
        else {
            msg_pat.mtype = 2;
            msg_pat.is_vip = COMMON;
        }

        size_t size = sizeof(msg_pat) - sizeof(long);
        int msg_send_pat_reg = msgsnd(msg_id_pat_reg, &msg_pat, size, 0);
        if(msg_send_pat_reg == -1){
            perror("[patient.c] error: msg_send_pat_reg");
            exit(1);
        }

        printf("|PATIENT %d| Data provided to registration!\n", getpid());
        sleep(2);
    }
    return 0;

}