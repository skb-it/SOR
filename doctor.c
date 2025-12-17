#include "common.h"

int main(){
    printf("|DOCTOR %d| Starting duty...", getpid());

    key_t key_msg_doc = ftok(FTOK_PATH, ID_MSG_DOCTOR);
    int msg_id_doc = msgget(key_msg_doc, 0);

    if(msg_id_doc == -1){
        perror("[doctor.c] error: msg_id_doc");
        exit(1);
    }

    printf("|DOCTOR %d| Waiting for patients...\n", getpid());

    while(1){
        sleep(10);
    }

    return 0;
}