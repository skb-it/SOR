#include "common.h"

int main(){
    printf("|PATIENT %d| Creating...\n", getpid());

    key_t key_msg_reg = ftok(FTOK_PATH, ID_MSG_REGISTRATION);
    int msg_id_reg = msgget(key_msg_reg, 0);
    if( msg_id_reg == -1){
        perror("[patient.c] error: msg_id_patient");
        exit(1);
    }

    struct Message msg_pat;
    msg_pat.mtype = 1;
    msg_pat.patient_id = getpid();
    msg_pat.age = 25;
    snprintf(msg_pat.description, 100, "My leg hurts");

    size_t size = sizeof(msg_pat) - sizeof(long);
    int msg_send = msgsnd(msg_id_reg, &msg_pat, size, 0);
    if(msg_send == -1){
        perror("[patient.c] error: msg_send");
        exit(1);
    }

    printf("|PATIENT %d| Message sent to registration!\n", getpid());

    return 0;
}