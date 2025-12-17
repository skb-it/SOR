#include "common.h"

int main(){
    printf("|REGISTRATION %d| Opening...", getpid());
    key_t key_msg_reg = ftok(FTOK_PATH, ID_MSG_REGISTRATION);
    int msg_reg_id = msgget(key_msg_reg, 0);
    if(msg_reg_id == -1){
        perror("[registration.c] error: msg_reg_id");
        exit(1);
    }

    key_t key_msg_doc = ftok(FTOK_PATH, ID_MSG_DOCTOR);
    int msg_doc_id = msgget(key_msg_doc, 0);
    if(msg_doc_id == -1){
        perror("[registration.c] error: msg_doc_id");
        exit(1);
    }

    printf("|REGISTRATION %d| Opened!", getpid());
    struct Message msg;
    size_t size = sizeof(msg) - sizeof(long);

    while(1){
        
        int msgrcv_reg_result = msgrcv(msg_reg_id, &msg, size, 0 , 0);
        if(msgrcv_reg_result  == -1){
            perror("[registration.c] error: msgrcv");
            exit(1);
        }

        printf("|REGISTRATION| A patient came! PID: %d, age: %d\n", msg.patient_id, msg.age);

        int r = rand() % 100;
        if(r < 10) { msg.mtype = TRIAGE_RED; }
        else if(r <= 45) {msg.mtype = TRIAGE_YELLOW; }
        else {msg.mtype = TRIAGE_RED; }
        
        printf("|REGISTRATION| Assigned priority: %ld\n", msg.mtype);

        int msgsnd_doc_result = msgsnd(msg_doc_id, &msg, size, 0);
        if(msgsnd_doc_result == -1) {
            perror("[registration.c] error: msgsnd_doc_result");
        }

        printf("|REGISTRATION| Patient forwarded to doctor.\n");

    }

    return 0;
}
