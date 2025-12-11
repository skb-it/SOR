#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>


int msg_id;
int shm_id;
int sem_id;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void clean_exit(int sig){
    msgctl(msg_id, IPC_RMID, NULL);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
    printf("The hospital is closed, resources are removed.\n");
    exit(0);
};

int main() {
    signal(SIGINT, clean_exit);

    printf("=====CREATING HOSPITAL=====");

    msg_id = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if(msg_id == -1){
        perror("msgget error (queue)");
        exit(1);
    }

    shm_id = shmget(SHM_KEY, sizeof(HospitalStatus), IPC_CREAT | 0666);
    if(shm_id == -1){
        perror("shmget error (memory)");
        exit(1);
    }
    printf("Memory created, ID: %d\n", shm_id);

    HospitalStatus *status = shmat(shm_id, NULL, 0);
    if(status == (void*)-1){
        perror("shmat error (connecting)");
        exit(1);
    }

    status->waiting_patients = 0;
    status->is_second_window_open = 0;
    status->simulation_end = 0;

    printf("Memory initialized (%d patients)\n", status->waiting_patients);

    shmdt(status);

    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget error (semaphore)");
        exit(1);
    }
    printf("Semaphore created, ID: %d\n", sem_id);

    union semun arg;
    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl error (semaphore initalization)");
        exit(1);
    }
    printf("Semaphore set on 1 (unlocked).\n");

     printf("\n Hospital is open and waiting for patients.\n");
    printf("Press Ctrl+C to close system and clean.\n");

    while(1) {
        sleep(1); 
    }

    return 0;

}