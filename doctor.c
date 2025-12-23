#include "common.h"

int main(){
    printf("|DOCTOR %d| Starting duty...\n", getpid());

    key_t key_msg_reg_doc = ftok(FTOK_PATH, ID_MSG_REG_DOC);
    if(key_msg_reg_doc == -1){
        perror("[doctor.c] error: key_msg_reg_doc");
        exit(1);
    }
    int msg_id_reg_doc = msgget(key_msg_reg_doc, 0);
    if(msg_id_reg_doc == -1){
        perror("[registration.c] error: msg_id_doc");
        exit(1);
    }


    struct Message msg_reg_doc;
    size_t size = sizeof(msg_reg_doc) - sizeof(long);

    while(1){
        printf("|DOCTOR %d| Waiting for a patient...\n", getpid());

        int msgrcv_reg_doc = msgrcv(msg_id_reg_doc, &msg_reg_doc, size, 0, 0);
        if(msgrcv_reg_doc == -1){
            perror("[doctor.c] error: msgrcv_reg_doc");
        }

        int triage_drawing = rand() % 100;
        if (triage_drawing < 10) {
            msg_reg_doc.mtype = TRIAGE_RED;
        } 
        else if (triage_drawing < 45) {
            msg_reg_doc.mtype = TRIAGE_YELLOW;
        } 
        else if (triage_drawing < 95) {
            msg_reg_doc.mtype = TRIAGE_GREEN;
        } 
        else {
            msg_reg_doc.mtype = SENT_HOME;
        }
        sleep(10);

    }

    return 0;
}