#include "common.h"
#include <signal.h>
#include <sys/wait.h>

int msg_id_pat_reg, msg_id_reg_doc, sem_id_waiting_room, shm_id;
pid_t reg1_pid = 0, reg2_pid = 0, triage_pid = 0;

void handle_sigint(int sig) {
    printf("\n|DIRECTOR| Closing hospital...\n");
    
    if(reg1_pid > 0) kill(reg1_pid, SIGTERM);
    if(reg2_pid > 0) kill(reg2_pid, SIGTERM);
    if(triage_pid > 0) kill(triage_pid, SIGTERM);

    msgctl(msg_id_pat_reg, IPC_RMID, NULL);
    msgctl(msg_id_reg_doc, IPC_RMID, NULL);
    semctl(sem_id_waiting_room, 0, IPC_RMID);
    shmctl(shm_id, IPC_RMID, NULL);
    
    printf("[|DIRECTOR| Hospital closed!\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    int N, K;
    printf("Set the size of waiting room (N): ");
    scanf("%d", &N);
    K = N / 2;

    key_t key_p = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    msg_id_pat_reg = msgget(key_p, 0600 | IPC_CREAT);

    key_t key_d = ftok(FTOK_PATH, ID_MSG_REG_DOC);
    msg_id_reg_doc = msgget(key_d, 0600 | IPC_CREAT);

    key_t key_s = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    sem_id_waiting_room = semget(key_s, 1, 0600 | IPC_CREAT);
    semctl(sem_id_waiting_room, 0, SETVAL, N); 

    reg1_pid = fork();
    if (reg1_pid == 0) {
        execl("./registration", "registration", "1", NULL);
        perror("[main.c] error: execl registration");
        exit(1);
    }

    triage_pid = fork();
    if (triage_pid == 0) {
        execl("./doctor", "doctor", "triage", NULL);
        perror("[main.c] error: execl doctor triage");
        exit(1);
    }

    printf("|DIRECTOR| Hospital works...\n");
 
    struct msqid_ds buf;
    while(1) {
        sleep(2);
        msgctl(msg_id_pat_reg, IPC_STAT, &buf);
        int patients_in_queue = buf.msg_qnum;

        if (patients_in_queue > K && reg2_pid == 0) {
            printf("|DIRECTOR| QUEUE > K (%d). Opening second registration.\n", patients_in_queue);
            reg2_pid = fork();
            if (reg2_pid == 0) {
                execl("./registration", "registration", "2", NULL);
                exit(0);
            }
        }

        if (patients_in_queue < N/3 && reg2_pid > 0) {
            printf("|DIRECTOR| Queue < N/3 (%d). Closing second registration.\n", patients_in_queue);
            kill(reg2_pid, SIGTERM);
            reg2_pid = 0;
        }
    }

    return 0;
}