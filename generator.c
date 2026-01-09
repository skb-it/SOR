#include "common.h"
#include "errors.h"

int main(){
    srand(time(NULL) ^ getpid());

    //GENERATING PATIENTS
    while(1){
        pid_t pat = fork();
        if(pat == 0){
            execl("./patient.c", "PATIENT", NULL);
            report_error("[generator.c] error: pat = fork()", 1);
        }

        
        int time = (rand() %3 ) + 1;
        sleep(time);
    }
    return 0;
}