#include "common.h"

int main(){

    key_t key_msg_reg = ftok(FTOK_PATH, ID_MSG_REGISTRATION);
    int msg_reg_id = msgget(key_msg_reg, 0);
    if(msg_reg_id == -1){
        perror("[registration.c] error: msg_reg_id");
        exit(1);
    }

    struct Message msg_reg;
    size_t size = sizeof(msg_reg) - sizeof(long);

    while(1){
        
        int msgrcv_result = msgrcv(msg_reg_id, &msg_reg,size, 0 , 0);
        if(msgrcv_result  == -1){
            perror("[registration.c] error: msgrcv");
            exit(1);
        }

        printf("|REGISTRATION| A patient came! PID: %d, age: %d\n", msg_reg.patient_id, msg_reg.age);
    }

    return 0;
}
