#include "common.h"
#include "errors.h"

int main(){
    

    srand(time(NULL) ^ getpid());

    //GENERATING PATIENTS
    while(1){
        while(waitpid(-1, NULL, WNOHANG) > 0){

        }

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