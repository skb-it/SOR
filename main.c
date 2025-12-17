#include "common.h"
#include <signal.h>

int msg_reg_id;
int msg_doc_id;
int semid_waiting_room;
int semctl_init;
int shm_id;

void handle_sigint(int sig){
    printf("Starting cleaning...\n");
    msgctl(msg_reg_id, IPC_RMID, NULL);
    msgctl(msg_doc_id, IPC_RMID, NULL);
    semctl(semid_waiting_room, 0, IPC_RMID);
    shmctl(shm_id, IPC_RMID, NULL);
    printf("Cleaning is complete. End.");
    
    exit(1);
}

int main(){

    signal(SIGINT, handle_sigint);

    printf("Set N value (size of the waiting room): ");
    int N;
    scanf("%d", &N);

    printf("\n\n-------------------------------------------------\n");
    printf("CREATING HOSPITAL\n");
    printf("-------------------------------------------------\n\n");


    msg_reg_id = msgget(KEY_MSG_REGISTRATION, 0600 | IPC_CREAT);
    if(msg_reg_id == -1) {
        perror("msg_reg_id error");
        exit(1);
     }

    msg_doc_id = msgget(KEY_MSG_DOCTOR, 0600 | IPC_CREAT);
    if(msg_doc_id == -1) { 
        perror("msg_doc_id error");
        exit(1);
     }

    semid_waiting_room = semget(KEY_SEM_WAITING_ROOM,1, 0600 | IPC_CREAT);
    if(semid_waiting_room == -1) {
        perror("semid_waiting_room error");
        exit(1);
    }

    semctl_init = semctl(semid_waiting_room, 0, SETVAL, N);
    if(semctl_init == -1){
        perror("semctl_init error");
        exit(1);
    }

    shm_id = shmget(KEY_SHM, SHM_SIZE, 0600 | IPC_CREAT);
    if(shm_id == -1) {
        perror("shm_id error");
        exit(1);
     }
     
    printf("-------------------------------------------------\n");
    printf("HOSPITAL CREATED\n");
    printf("-------------------------------------------------\n\n");

    printf("-----Press CTRL+C to end simulation and clean----\n\n");

    while(1){
        sleep(1);
    }

    return 0;
}