#include "common.h"
#include "errors.h"

void fill_pat_data(struct MsgBuffer *buf, int age){

    //GETING PATIENT PID
    buf->patient_id = getpid();

    //GETTING PATIENT AGE
    buf->age ;
    
    //DRAWING - VIP(CHANCE OF 15%) OR COMMON(CHANCE OF 85%)
    if ((rand() % 100) < 15) buf->mtype = VIP;
    else                     buf->mtype = COMMON;
}


int main(){

    //GENERATING PATIENT AGE AND CHECKING IF GUARDIAN IS REQUIRED
    srand(time(NULL) ^ getpid());
    int age = rand() % 117;

    if(age < 18){
        printf("[GUARDIAN %d] Coming with child (age: %d).\n", getpid(), age);

        pid_t pid = fork();

        if(pid == -1) report_error("[patient.c] error: fork()", 1);

        //GUARDIAN PROCESS
        if(pid > 0){
            wait(NULL);

            printf("[GUARDIAN %d] My child is examinated. Leaving ER with him...", getpid());
        }

        //CHILD PROCESS
        else{

        }
    }
                //GUARDIAN+CHILD PROCESS


    //SHARED MEMORY MAIN.C->PATIENT.C
    key_t key_N = ftok(FTOK_PATH, ID_SHM_N);
    if(key_N == -1) report_error("[patient.c] error: key_N", 1);

    int shmget_N = shmget(key_N, SHM_SIZE_INT, 0600 | IPC_CREAT);
    if(shmget_N == -1) report_error("[patient.c] error: shmget_N", 1);

    struct Data *N = (struct Data*) shmat(shmget_N, NULL, 0);
    if(N == (void *)-1) report_error("[patient.c] error: shmat (N)", 1);


    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[patient.c] error: key_shm_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[patient.c] error: shmget_pat_reg", 1);

    struct MsgBuffer buf;

    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);


    //FILLING PATIENT DATA
    fill_pat_data(&buf, age);

    //SHARING PATIENT DATA FOR REGISTRATION
    int msgsnd_pat_reg = msgsnd(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), 0);
    if(msgsnd_pat_reg == -1) report_error("[patient.c] error: msgsnd_pat_reg", 1);

    




    //DETACHING SHARED MEMORY (N)
    int shmdt_N = shmdt(N);
    if(shmdt_N == -1) report_error("[patient.c] error: shmdt_N", 1);


    return 0;
}