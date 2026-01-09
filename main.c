#include "common.h"
#include "errors.h"


int main(){
    printf("Please enter N value (size of the waiting room):");

    //SHARED MEMORY FOR N
    key_t key_N = ftok(FTOK_PATH, ID_SHM_N);
    if(key_N == -1) report_error("[main.c] error: key_N", 1);

    int shmget_N = shmget(key_N, SHM_SIZE_INT, 0600 | IPC_CREAT);
    if(shmget_N == -1) report_error("[main.c] error: shmget_N", 1);

    struct Data *N = (struct Data*) shmat(shmget_N, NULL, 0);
    if(N == (void *)-1) report_error("[main.c] error: shmat (N)", 1);
    scanf("%d", &N->N);




    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, N->N, 0600 | IPC_CREAT);
    if(semget_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);

    for(int i = 0; i<N->N; i++){
        semctl(semget_waiting_room, i, SETVAL, 1);
    }
    
    int K;

    while(1){
        sleep(5);
    }




    //CLEANING SHARED MEMORY (N)
    int shmctl_del_N = shmctl(shmget_N, IPC_RMID, NULL);
    if(shmctl_del_N == -1) report_error("[main.c] error: shmctl_del_N", 1);

    return 0;
}
