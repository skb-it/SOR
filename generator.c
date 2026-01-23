#include "common.h"
#include "errors.h"

volatile int gen_running = 1;

void handle_gen_signal(int sig){
    gen_running = 0;
}

void is_there_place(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    while(semop(semget, &sb, 1) == -1) {
        if(errno == EINTR) {
            if(!gen_running) return;
            continue;
        }
        report_error("[generator.c] semop_gen", 1);
    }
}

int main(){
    srand(time(NULL) ^ getpid());
    signal(SIGTERM, handle_gen_signal);

    //SEMAPHORE GENERATOR
    key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen == -1) report_error("[director.c] key_sem_gen", 1);

    int semget_gen = semget(key_sem_gen, 1, 0600 | IPC_CREAT);
    if(semget_gen == -1) report_error("[director.c] semget_gen", 1);

    //GENERATING PATIENTS
    while(gen_running){
        is_there_place(semget_gen);
        if(!gen_running) break;

        pid_t pat = fork();
        if(pat == 0){
            execl("./patient", "patient", NULL);
            report_error("[generator.c] pat = fork()", 1);
        }
        else if (pat > 0) {
            increment_total_patients();
        }
        else if (pat == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                if(gen_running) wait(NULL);
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