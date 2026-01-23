#include "common.h"
#include "errors.h"
#include <asm-generic/signal.h>

void handle_zombies(int sig) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}



int main(){
    
    struct sigaction sa;
    sa.sa_handler = handle_zombies;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    srand(time(NULL) ^ getpid());

    //GENERATING PATIENTS
    while(1){
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