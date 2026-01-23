#include "common.h"
#include "errors.h"
#include <asm-generic/signal.h>

void handle_zombies(int sig) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void is_there_place(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    int semop_gen = semop(semget, &sb, 1);
    if(semop_gen == -1) report_error("[patient.c] semop_gen", 1);
}

int main(){
    
    struct sigaction sa;
    sa.sa_handler = handle_zombies;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    srand(time(NULL) ^ getpid());

    //SEMAPHORE GENERATOR
    key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen == -1) report_error("[director.c] key_sem_gen", 1);

    int semget_gen = semget(key_sem_gen, 1, 0600 | IPC_CREAT);
    if(semget_gen == -1) report_error("[director.c] semget_gen", 1);

    //GENERATING PATIENTS
    while(1){
        is_there_place(semget_gen);

        pid_t pat = fork();
        if(pat == 0){
            execl("./patient", "patient", NULL);
            report_error("[generator.c] pat = fork()", 1);
        }
        else if (pat == -1) {
            if (errno == EAGAIN) {
                wait(NULL);
                continue;
            }
            else {
                report_error("[generator.c] pat = fork()", 1);
            }
        }
        
        //int time = (rand() %3 ) + 1;
        //sleep(time);
    }
    return 0;
}