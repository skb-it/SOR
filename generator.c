#include "common.h"
#include "errors.h"
#include <pthread.h>
#include <sys/signalfd.h>



volatile sig_atomic_t gen_running = 1;
volatile sig_atomic_t killer_running = 1;

int signal_fd = -1;

void handle_gen_signal(int sig){
    (void)sig;
    gen_running = 0;
    killer_running = 0;
}

void is_there_place(int semget_id){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    while(semop(semget_id, &sb, 1) == -1) {
        if(errno == EINTR) {
            if(!gen_running) return;
            continue;
        }
        report_error("[generator.c] semop_gen", 1);
    }
}

void* zombie_killer(void* arg) {
    (void)arg;
    
    struct signalfd_siginfo fdsi;
    
    LOG_PRINTF("[REAPER %d] Zombie killer thread started.", getpid());
    
    while(killer_running) {
        int s = read(signal_fd, &fdsi, sizeof(fdsi));
        
        if(s != sizeof(fdsi)) {
            if(errno == EBADF || !killer_running) break;
            if(errno == EINTR || errno == EAGAIN) continue;
            break;
        }
        
        pid_t pid;
        int status;
        while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        }
    }
    
    pid_t pid;
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0);
    
    LOG_PRINTF("[REAPER %d] Zombie killer thread finished.", getpid());
    return NULL;
}

int main(){
    srand(time(NULL) ^ getpid());
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    
    if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        report_error("[generator.c] sigprocmask", 1);
    }
    
    signal_fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if(signal_fd == -1) {
        report_error("[generator.c] signalfd", 1);
    }
    
    struct sigaction sa;
    sa.sa_handler = handle_gen_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    pthread_t killer_thread;
    if(pthread_create(&killer_thread, NULL, zombie_killer, NULL) != 0) {
        report_error("[generator.c] pthread_create killer", 1);
    }

    key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen == -1) report_error("[generator.c] key_sem_gen", 1);

    int semget_gen = semget(key_sem_gen, 1, 0600);
    if(semget_gen == -1) report_error("[generator.c] semget_gen", 1);

    LOG_PRINTF("[GENERATOR %d] Started.", getpid());

    // GENERATING PATIENTS
    while(gen_running){
        is_there_place(semget_gen);
        if(!gen_running) break;

        pid_t pat = fork();
        if(pat == 0){
            close(signal_fd);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            execl("./patient", "patient", NULL);
            report_error("[generator.c] execl patient", 1);
        }
        else if (pat > 0) {
            increment_total_patients();
        }
        else if (pat == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            else {
                report_error("[generator.c] fork()", 1);
            }
        }
    }

    LOG_PRINTF("[GENERATOR %d] Stopped generating, waiting for children to finish...", getpid());
    
    sigset_t mask_unblock;
    sigemptyset(&mask_unblock);
    sigaddset(&mask_unblock, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &mask_unblock, NULL);
    
    pid_t pid;
    int status;
    while((pid = wait(&status)) > 0 || (pid == -1 && errno == EINTR)) {
    }

    killer_running = 0;
    
    if(signal_fd != -1) {
        close(signal_fd);
        signal_fd = -1;
    }
    
    pthread_join(killer_thread, NULL);

    LOG_PRINTF("[GENERATOR %d] Shutting down.", getpid());
    return 0;
}