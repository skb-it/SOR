#include "common.h"
#include <signal.h>

int msg_reg_id;
int msg_doc_id;
int sem_id_waiting_room;
int semctl_init;
int shm_id;

void handle_sigint(int sig){
    printf("Starting cleaning...\n");
    msgctl(msg_reg_id, IPC_RMID, NULL);
    msgctl(msg_doc_id, IPC_RMID, NULL);
    semctl(sem_id_waiting_room, 0, IPC_RMID);
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


    //MESSAGE QUEUE - REGISTRATION
    key_t key_msg_reg = ftok(FTOK_PATH, ID_MSG_REGISTRATION);
    if(key_msg_reg == -1){
        perror("[main.c] error: key_msg_reg");
        exit(1);
    }
    msg_reg_id = msgget(key_msg_reg, 0600 | IPC_CREAT);
    if(msg_reg_id == -1) {
        perror("[main.c] error: msg_reg_id");
        exit(1);
     }


    //MESSAGE QUQUE - DOCTOR
    key_t key_msg_doc = ftok(FTOK_PATH, ID_MSG_DOCTOR);
    if(key_msg_doc == -1){
        perror("[main.c] error: key_msg_doc");
        exit(1);
    }
    msg_doc_id = msgget(key_msg_doc, 0600 | IPC_CREAT);
    if(msg_doc_id == -1) { 
        perror("[main.c] error: msg_doc_id");
        exit(1);
     }


    //SEMAPHORES
    key_t key_sem = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem == -1){
        perror("[main.c] error: key_sem");
        exit(1);
    }
    sem_id_waiting_room = semget(key_sem, 1, 0600 | IPC_CREAT);
    if(sem_id_waiting_room == -1) {
        perror("[main.c] error: semid_waiting_room");
        exit(1);
    }
    semctl_init = semctl(sem_id_waiting_room, 0, SETVAL, N);
    if(semctl_init == -1){
        perror("[main.c] error: semctl_init");
        exit(1);
    }


    //SHARED MEMORY
    key_t key_shm = ftok(FTOK_PATH, ID_SHM);
    shm_id = shmget(key_shm, SHM_SIZE, 0600 | IPC_CREAT);
    if(shm_id == -1) {
        perror("[main.c] error: shm_id");
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