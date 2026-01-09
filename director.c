#include "common.h"
#include "errors.h"
#include <signal.h>

volatile sig_atomic_t is_ER_open = 1;

void evacuation(){
    printf("[DIRECTOR] Evacuation of the emergency room!");

                //KILLING PROCESSES
}

int main(){
    //CTRL+C = EVACUATION OF ER
    signal(SIGINT, evacuation);
    
    printf("[DIRECTOR] Opening ER...");

    //OPENING REGISTRATION
    pid_t reg = fork();
    if(reg == 0){
        execl("./registration.c", "REGISTRATION", NULL);
        report_error("[director.c] error: reg=fork()", 1);
    }

    //HIRING PRIMARY CARE DOCTORS
    for(int i=0; i<3; i++){
        pid_t doc = fork();
        if(doc == 0){
            execl("./pc_doctor.c", "DOCTOR", NULL);
            report_error("[director.c] error: doc=fork()", 1);
        }
    }

    pid_t pat_gen = fork();
    if(pat_gen == 0){
        execl("./generator.c", "GENERATOR", NULL);
        report_error("[director.c] error: pat_reg = fork()",1);
    }

    while(is_ER_open){
        sleep(1);
    }

    printf("[DIRECTOR] Closing and cleaning ER...");
                //CLEANING
    return 0;
}
