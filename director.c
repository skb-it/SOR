#include "common.h"
#include "errors.h"
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t is_ER_open = 1;

int g_shmget_reg_doc = -1;
int g_semget_waiting_room = -1;
int g_semget_msg_pat_reg = -1;
int g_semget_doc = -1;
int g_semget_msg_pat_doc = -1;
int g_semget_msg_pat_cardio = -1;
int g_semget_msg_pat_eyedoc = -1;
int g_semget_msg_pat_laryng = -1;
int g_semget_msg_pat_neuro = -1;
int g_semget_msg_pat_pediatr = -1;
int g_semget_msg_pat_surgeon = -1;
int g_semget_gen = -1;
int g_semget_log = -1;
int g_shm_stats = -1;
int g_semget_stats = -1;
int g_msg_pat_reg = -1;
int g_msg_doc_pat = -1;
int g_msg_pat_cardio = -1;
int g_msg_pat_neuro = -1;
int g_msg_pat_eye = -1;
int g_msg_pat_laryng = -1;
int g_msg_pat_surgeon = -1;
int g_msg_pat_pediatr = -1;

pid_t g_pids[10];

void handle_sigchld(int sig) {
    (void)sig;
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void cleanup_ipc() {
    if (g_msg_pat_reg >= 0) {
        int msgctl_pat_reg = msgctl(g_msg_pat_reg, IPC_RMID, NULL);
        if (msgctl_pat_reg == -1) {
            report_error("[director.c] msgctl_pat_reg", 1);
        }
        g_msg_pat_reg = -1;
    }
    if (g_msg_doc_pat >= 0) {
        int msgctl_doc_pat = msgctl(g_msg_doc_pat, IPC_RMID, NULL);
        if (msgctl_doc_pat == -1) {
            report_error("[director.c] msgctl_doc_pat", 1);
        }
        g_msg_doc_pat = -1;
    }
    if (g_msg_pat_cardio >= 0) {
        int msgctl_pat_cardio = msgctl(g_msg_pat_cardio, IPC_RMID, NULL);
        if (msgctl_pat_cardio == -1) {
            report_error("[director.c] msgctl_pat_cardio", 1);
        }
        g_msg_pat_cardio = -1;
    }
    if (g_msg_pat_neuro >= 0) {
        int msgctl_pat_neuro = msgctl(g_msg_pat_neuro, IPC_RMID, NULL);
        if (msgctl_pat_neuro == -1) {
            report_error("[director.c] msgctl_pat_neuro", 1);
        }
        g_msg_pat_neuro = -1;
    }
    if (g_msg_pat_eye >= 0) {
        int msgctl_pat_eye = msgctl(g_msg_pat_eye, IPC_RMID, NULL);
        if (msgctl_pat_eye == -1) {
            report_error("[director.c] msgctl_pat_eye", 1);
        }
        g_msg_pat_eye = -1;
    }
    if (g_msg_pat_laryng >= 0) {
        int msgctl_pat_laryng = msgctl(g_msg_pat_laryng, IPC_RMID, NULL);
        if (msgctl_pat_laryng == -1) {
            report_error("[director.c] msgctl_pat_laryng", 1);
        }
        g_msg_pat_laryng = -1;
    }
    if (g_msg_pat_surgeon >= 0) {
        int msgctl_pat_surgeon = msgctl(g_msg_pat_surgeon, IPC_RMID, NULL);
        if (msgctl_pat_surgeon == -1) {
            report_error("[director.c] msgctl_pat_surgeon", 1);
        }
        g_msg_pat_surgeon = -1;
    }
    if (g_msg_pat_pediatr >= 0) {
        int msgctl_pat_pediatr = msgctl(g_msg_pat_pediatr, IPC_RMID, NULL);
        if (msgctl_pat_pediatr == -1) {
            report_error("[director.c] msgctl_pat_pediatr", 1);
        }
        g_msg_pat_pediatr = -1;
    }
    if (g_shmget_reg_doc >= 0) {
        int shmctl_reg_doc = shmctl(g_shmget_reg_doc, IPC_RMID, NULL);
        if (shmctl_reg_doc == -1) {
            report_error("[director.c] shmctl_reg_doc", 1);
        }
        g_shmget_reg_doc = -1;
    }
    if (g_shm_stats >= 0) {
        int shmctl_stats = shmctl(g_shm_stats, IPC_RMID, NULL);
        if (shmctl_stats == -1) {
            report_error("[director.c] shmctl_stats", 1);
        }
        g_shm_stats = -1;
    }
    if (g_semget_waiting_room >= 0) {
        int semctl_waiting_room = semctl(g_semget_waiting_room, 0, IPC_RMID);
        if (semctl_waiting_room == -1) {
            report_error("[director.c] semctl_waiting_room", 1);
        }
        g_semget_waiting_room = -1;
    }
    if (g_semget_msg_pat_reg >= 0) {
        int semctl_msg_pat_reg = semctl(g_semget_msg_pat_reg, 0, IPC_RMID);
        if (semctl_msg_pat_reg == -1) {
            report_error("[director.c] semctl_msg_pat_reg", 1);
        }
        g_semget_msg_pat_reg = -1;
    }
    if (g_semget_doc >= 0) {
        int semctl_doc = semctl(g_semget_doc, 0, IPC_RMID);
        if (semctl_doc == -1) {
            report_error("[director.c] semctl_doc", 1);
        }
        g_semget_doc = -1;
    }
    if (g_semget_msg_pat_doc >= 0) {
        int semctl_msg_pat_doc = semctl(g_semget_msg_pat_doc, 0, IPC_RMID);
        if (semctl_msg_pat_doc == -1) {
            report_error("[director.c] semctl_msg_pat_doc", 1);
        }
        g_semget_msg_pat_doc = -1;
    }
    if (g_semget_msg_pat_cardio >= 0) {
        int semctl_msg_pat_cardio = semctl(g_semget_msg_pat_cardio, 0, IPC_RMID);
        if (semctl_msg_pat_cardio == -1) {
            report_error("[director.c] semctl_msg_pat_cardio", 1);
        }
        g_semget_msg_pat_cardio = -1;
    }
    if (g_semget_msg_pat_eyedoc >= 0) {
        int semctl_msg_pat_eyedoc = semctl(g_semget_msg_pat_eyedoc, 0, IPC_RMID);
        if (semctl_msg_pat_eyedoc == -1) {
            report_error("[director.c] semctl_msg_pat_eyedoc", 1);
        }
        g_semget_msg_pat_eyedoc = -1;
    }
    if (g_semget_msg_pat_laryng >= 0) {
        int semctl_msg_pat_laryng = semctl(g_semget_msg_pat_laryng, 0, IPC_RMID);
        if (semctl_msg_pat_laryng == -1) {
            report_error("[director.c] semctl_msg_pat_laryng", 1);
        }
        g_semget_msg_pat_laryng = -1;
    }
    if (g_semget_msg_pat_neuro >= 0) {
        int semctl_msg_pat_neuro = semctl(g_semget_msg_pat_neuro, 0, IPC_RMID);
        if (semctl_msg_pat_neuro == -1) {
            report_error("[director.c] semctl_msg_pat_neuro", 1);
        }
        g_semget_msg_pat_neuro = -1;
    }
    if (g_semget_msg_pat_pediatr >= 0) {
        int semctl_msg_pat_pediatr = semctl(g_semget_msg_pat_pediatr, 0, IPC_RMID);
        if (semctl_msg_pat_pediatr == -1) {
            report_error("[director.c] semctl_msg_pat_pediatr", 1);
        }
        g_semget_msg_pat_pediatr = -1;
    }
    if (g_semget_msg_pat_surgeon >= 0) {
        int semctl_msg_pat_surgeon = semctl(g_semget_msg_pat_surgeon, 0, IPC_RMID);
        if (semctl_msg_pat_surgeon == -1) {
            report_error("[director.c] semctl_msg_pat_surgeon", 1);
        }
        g_semget_msg_pat_surgeon = -1;
    }
    if (g_semget_gen >= 0) {
        int semctl_gen = semctl(g_semget_gen, 0, IPC_RMID);
        if (semctl_gen == -1) {
            report_error("[director.c] semctl_gen", 1);
        }
        g_semget_gen = -1;
    }
    if (g_semget_log >= 0) {
        int semctl_log = semctl(g_semget_log, 0, IPC_RMID);
        if (semctl_log == -1) {
            report_error("[director.c] semctl_log", 1);
        }
        g_semget_log = -1;
    }
    if (g_semget_stats >= 0) {
        int semctl_stats = semctl(g_semget_stats, 0, IPC_RMID);
        if (semctl_stats == -1) {
            report_error("[director.c] semctl_stats", 1);
        }
        g_semget_stats = -1;
    }
}

void cleanup_processes() {

    if(g_pids[2] > 0) {
        kill(g_pids[2], SIGTERM);
        waitpid(g_pids[2], NULL, WNOHANG);
        g_pids[2] = 0;
    }
    for(int i = 0; i < 10; i++) {
        if(g_pids[i] > 0) {
            kill(g_pids[i], SIGTERM);
        }
    }

    int remaining_processes = 0;
    for(int i = 0; i < 10; i++) {
        if(g_pids[i] > 0) {
            int status;
            if(waitpid(g_pids[i], &status, WNOHANG) == 0) {
                remaining_processes++;
            } else {
                g_pids[i] = 0;
            }
        }
    }
    
    if(remaining_processes > 0) {
        for(int i = 0; i < 10; i++) {
            if(g_pids[i] > 0) {
                kill(g_pids[i], SIGKILL);
            }
        }
        
        for(int i = 0; i < 10; i++) {
            if(g_pids[i] > 0) {
                waitpid(g_pids[i], NULL, WNOHANG);
                g_pids[i] = 0;
            }
        }
    }
    
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void cleanup() {
    LOG_PRINTF("|DIRECTOR| Starting cleanup...");
    cleanup_processes();
    cleanup_ipc();
}

void evacuation(int sig) {
    (void)sig;
    is_ER_open = 0;
}

int init_shared_memory() {
    //SHARED MEMORY REGISTRATION<->PRIMARY CARE DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[director.c] key_shm_reg_doc", 1);

    g_shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard), 0600 | IPC_CREAT);
    if(g_shmget_reg_doc == -1) report_error("[director.c] g_shmget_reg_doc", 1);

    //SHARED MEMORY STATS
    key_t key_shm_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_shm_stats == -1) report_error("[director.c] key_shm_stats", 1);

    g_shm_stats = shmget(key_shm_stats, sizeof(struct PatientStats), 0600 | IPC_CREAT);
    if(g_shm_stats == -1) report_error("[director.c] g_shm_stats", 1);

    struct PatientStats *stats = shmat(g_shm_stats, NULL, 0);
    if(stats == (void *)-1) report_error("[director.c] stats", 1);
    
    //SETTING WHOLE STRUCTURE TO 0
    memset(stats, 0, sizeof(struct PatientStats));

    if(shmdt(stats) == -1) report_error("[director.c] shmdt stats init", 0);

    return 0;
}

int init_semaphores(int N) {
    union semun sem;

    // SEMAPHORE STATS
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) report_error("[director.c] key_sem_stats", 1);
    g_semget_stats = semget(key_sem_stats, 1, 0600 | IPC_CREAT);
    if(g_semget_stats == -1) report_error("[director.c] g_semget_stats", 1);
    sem.val = 1;
    int semctl_stats = semctl(g_semget_stats, 0, SETVAL, sem);
    if(semctl_stats == -1) report_error("[director.c] semctl_stats", 1);

    // SEMAPHORE LOG FILE
    key_t key_sem_log = ftok(FTOK_PATH, ID_SEM_LOG_FILE);
    if(key_sem_log == -1) report_error("[director.c] key_sem_log", 1);
    g_semget_log = semget(key_sem_log, 1, 0600 | IPC_CREAT);
    if(g_semget_log == -1) report_error("[director.c] g_semget_log", 1);
    sem.val = 1;
    int semctl_log = semctl(g_semget_log, 0, SETVAL, sem);
    if(semctl_log == -1) report_error("[director.c] semctl_log", 1);

    // SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[director.c] key_sem_waiting_room", 1);
    g_semget_waiting_room = semget(key_sem_waiting_room, 1, 0600 | IPC_CREAT);
    if(g_semget_waiting_room == -1) report_error("[director.c] g_semget_waiting_room", 1);
    sem.val = N;
    int semctl_waiting_room = semctl(g_semget_waiting_room, 0, SETVAL, sem);
    if(semctl_waiting_room == -1) report_error("[director.c] semctl_waiting_room", 1);

    // SEMAPHORE MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[director.c] key_sem_msg_pat_reg", 1);
    g_semget_msg_pat_reg = semget(key_sem_msg_pat_reg, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_reg == -1) report_error("[director.c] g_semget_msg_pat_reg", 1);
    sem.val = 1020;
    int semctl_msg_pat_reg = semctl(g_semget_msg_pat_reg, 0, SETVAL, sem);
    if(semctl_msg_pat_reg == -1) report_error("[director.c] semctl_msg_pat_reg", 1);

    // SEMAPHORE REGISTRATION<->DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[director.c] key_sem_doc", 1);
    g_semget_doc = semget(key_sem_doc, 2, 0600 | IPC_CREAT);
    if(g_semget_doc == -1) report_error("[director.c] g_semget_doc", 1);
    sem.val = 1;  //FREE SLOT
    int semctl_sem_doc_0 = semctl(g_semget_doc, 0, SETVAL, sem);
    if(semctl_sem_doc_0 == -1) report_error("[director.c] semctl_doc_0", 1);
    sem.val = 0;  //CARD IS READY
    int semctl_sem_doc_1 =semctl(g_semget_doc, 1, SETVAL, sem);
    if(semctl_sem_doc_1 == -1) report_error("[director.c] semctl_doc_1", 1);

    // SEMAPHORES FOR SPECIALIST QUEUES
    sem.val = 500;
    key_t key_sem_msg_pat_doc = ftok(FTOK_PATH, ID_SEM_MSG_PAT_DOC);
    if(key_sem_msg_pat_doc == -1) report_error("[director.c] key_sem_msg_pat_doc", 1);
    g_semget_msg_pat_doc = semget(key_sem_msg_pat_doc, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_doc == -1) report_error("[director.c] g_semget_msg_pat_doc", 1);
    int semctl_msg_pat_doc = semctl(g_semget_msg_pat_doc, 0, SETVAL, sem);
    if(semctl_msg_pat_doc == -1) report_error("[director.c] semctl_msg_pat_doc", 1);

    key_t key_sem_msg_pat_cardio = ftok(FTOK_PATH, ID_SEM_MSG_CARDIO);
    if(key_sem_msg_pat_cardio == -1) report_error("[director.c] key_sem_msg_pat_cardio", 1);
    g_semget_msg_pat_cardio = semget(key_sem_msg_pat_cardio, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_cardio == -1) report_error("[director.c] g_semget_msg_pat_cardio", 1);
    int semctl_msg_pat_cardio = semctl(g_semget_msg_pat_cardio, 0, SETVAL, sem);
    if(semctl_msg_pat_cardio == -1) report_error("[director.c] semctl_msg_pat_cardio", 1);

    key_t key_sem_msg_pat_eyedoc = ftok(FTOK_PATH, ID_SEM_MSG_EYEDOC);
    if(key_sem_msg_pat_eyedoc == -1) report_error("[director.c] key_sem_msg_pat_eyedoc", 1);
    g_semget_msg_pat_eyedoc = semget(key_sem_msg_pat_eyedoc, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_eyedoc == -1) report_error("[director.c] g_semget_msg_pat_eyedoc", 1);
    int semctl_msg_pat_eyedoc = semctl(g_semget_msg_pat_eyedoc, 0, SETVAL, sem);
    if(semctl_msg_pat_eyedoc == -1) report_error("[director.c] semctl_msg_pat_eyedoc", 1);

    key_t key_sem_msg_pat_laryng = ftok(FTOK_PATH, ID_SEM_MSG_LARYNG);
    if(key_sem_msg_pat_laryng == -1) report_error("[director.c] key_sem_msg_pat_laryng", 1);
    g_semget_msg_pat_laryng = semget(key_sem_msg_pat_laryng, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_laryng == -1) report_error("[director.c] g_semget_msg_pat_laryng", 1);
    int semctl_msg_pat_laryng = semctl(g_semget_msg_pat_laryng, 0, SETVAL, sem);
    if(semctl_msg_pat_laryng == -1) report_error("[director.c] semctl_msg_pat_laryng", 1);

    key_t key_sem_msg_pat_neuro = ftok(FTOK_PATH, ID_SEM_MSG_NEURO);
    if(key_sem_msg_pat_neuro == -1) report_error("[director.c] key_sem_msg_pat_neuro", 1);
    g_semget_msg_pat_neuro = semget(key_sem_msg_pat_neuro, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_neuro == -1) report_error("[director.c] g_semget_msg_pat_neuro", 1);
    int semctl_msg_pat_neuro = semctl(g_semget_msg_pat_neuro, 0, SETVAL, sem);
    if(semctl_msg_pat_neuro == -1) report_error("[director.c] semctl_msg_pat_neuro", 1);

    key_t key_sem_msg_pat_pediatr = ftok(FTOK_PATH, ID_SEM_MSG_PEDIATR);
    if(key_sem_msg_pat_pediatr == -1) report_error("[director.c] key_sem_msg_pat_pediatr", 1);
    g_semget_msg_pat_pediatr = semget(key_sem_msg_pat_pediatr, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_pediatr == -1) report_error("[director.c] g_semget_msg_pat_pediatr", 1);
    int semctl_msg_pat_pediatr = semctl(g_semget_msg_pat_pediatr, 0, SETVAL, sem);
    if(semctl_msg_pat_pediatr == -1) report_error("[director.c] semctl_msg_pat_pediatr", 1);

    key_t key_sem_msg_pat_surgeon = ftok(FTOK_PATH, ID_SEM_MSG_SURGEON);
    if(key_sem_msg_pat_surgeon == -1) report_error("[director.c] key_sem_msg_pat_surgeon", 1);
    g_semget_msg_pat_surgeon = semget(key_sem_msg_pat_surgeon, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_surgeon == -1) report_error("[director.c] g_semget_msg_pat_surgeon", 1);
    int semctl_msg_pat_surgeon = semctl(g_semget_msg_pat_surgeon, 0, SETVAL, sem);
    if(semctl_msg_pat_surgeon == -1) report_error("[director.c] semctl_msg_pat_surgeon", 1);

    // SEMAPHORE GENERATOR
    key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen == -1) report_error("[director.c] key_sem_gen", 1);
    g_semget_gen = semget(key_sem_gen, 1, 0600 | IPC_CREAT);
    if(g_semget_gen == -1) report_error("[director.c] g_semget_gen", 1);
    sem.val = 10000;
    int semctl_gen = semctl(g_semget_gen, 0, SETVAL, sem);
    if(semctl_gen == -1) report_error("[director.c] semctl_gen", 1);
    return 0;
}

int init_message_queues() {
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[director.c] key_msg_pat_reg", 1);
    g_msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(g_msg_pat_reg == -1) report_error("[director.c] g_msg_pat_reg", 1);

    key_t key_msg_pat_doc= ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_pat_doc==-1) report_error("[director.c] key_msg_doc_pat", 1);
    g_msg_doc_pat = msgget(key_msg_pat_doc, 0600 | IPC_CREAT);
    if(g_msg_doc_pat == -1) report_error("[director.c] g_msg_doc_pat", 1);

    key_t key_msg_pat_cardio= ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio==-1) report_error("[director.c] key_msg_pat_cardio", 1);
    g_msg_pat_cardio = msgget(key_msg_pat_cardio, 0600 | IPC_CREAT);
    if(g_msg_pat_cardio == -1) report_error("[director.c] g_msg_pat_cardio", 1);

    key_t key_msg_pat_neuro= ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro==-1) report_error("[director.c] key_msg_pat_neuro", 1);
    g_msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(g_msg_pat_neuro == -1) report_error("[director.c] g_msg_pat_neuro", 1);

    key_t key_msg_pat_eye= ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eye==-1) report_error("[director.c] key_msg_pat_eye", 1);
    g_msg_pat_eye = msgget(key_msg_pat_eye, 0600 | IPC_CREAT);
    if(g_msg_pat_eye == -1) report_error("[director.c] g_msg_pat_eye", 1);

    key_t key_msg_pat_laryng= ftok(FTOK_PATH, ID_MSG_PAT_LARYNG);
    if(key_msg_pat_laryng==-1) report_error("[director.c] key_msg_pat_laryng", 1);
    g_msg_pat_laryng = msgget(key_msg_pat_laryng, 0600 | IPC_CREAT);
    if(g_msg_pat_laryng == -1) report_error("[director.c] g_msg_pat_laryng", 1);

    key_t key_msg_pat_surgeon= ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon==-1) report_error("[director.c] key_msg_pat_surgeon", 1);
    g_msg_pat_surgeon = msgget(key_msg_pat_surgeon, 0600 | IPC_CREAT);
    if(g_msg_pat_surgeon == -1) report_error("[director.c] g_msg_pat_surgeon", 1);

    key_t key_msg_pat_pediatr= ftok(FTOK_PATH, ID_MSG_PAT_PEDIATR);
    if(key_msg_pat_pediatr==-1) report_error("[director.c] key_msg_pat_pediatr", 1);
    g_msg_pat_pediatr = msgget(key_msg_pat_pediatr, 0600 | IPC_CREAT);
    if(g_msg_pat_pediatr == -1) report_error("[director.c] g_msg_pat_pediatr", 1);

    return 0;
}

int start_staff() {
    // REGISTRATION
    g_pids[0] = fork();
    if(g_pids[0] == -1) {
        cleanup(); 
        report_error("[director.c] fork registration", 1); 
    }
    if(g_pids[0] == 0) {
        execl("./registration", "registration", NULL);
        cleanup();
        report_error("[director.c] execl registration", 1);
    }

    //2ND REGISTRATION
    g_pids[1] = 0;

    // GENERATOR
    g_pids[2] = fork();
    if(g_pids[2] == -1) {
        cleanup();
        report_error("[director.c] fork generator", 1);
    }
    if(g_pids[2] == 0) {
        execl("./generator", "generator", NULL);
        cleanup();
        report_error("[director.c] execl generator", 1);
    }

    // CARDIOLOGIST
    g_pids[3] = fork();
    if(g_pids[3] == -1) {
        cleanup(); 
        report_error("[director.c] fork cardiologist", 1); }
    if(g_pids[3] == 0) {
        execl("./cardiologist", "cardiologist", NULL);
        cleanup();
        report_error("[director.c] execl cardiologist", 1);
    }

    // NEUROLOGIST
    g_pids[4] = fork();
    if(g_pids[4] == -1) {
        cleanup();
        report_error("[director.c] fork neurologist", 1);
    }
    if(g_pids[4] == 0) {
        execl("./neurologist", "neurologist", NULL);
        cleanup();
        report_error("[director.c] execl neurologist", 1);
    }

    // EYE DOCTOR
    g_pids[5] = fork();
    if(g_pids[5] == -1) {
        cleanup();
        report_error("[director.c] fork eyedoc", 1);
    }
    if(g_pids[5] == 0) {
        execl("./eyedoc", "eyedoc", NULL);
        cleanup();
        report_error("[director.c] execl eyedoc", 1);
    }

    // LARYNGOLOGIST
    g_pids[6] = fork();
    if(g_pids[6] == -1) {
        cleanup();
        report_error("[director.c] fork laryngologist", 1); }
    if(g_pids[6] == 0) {
        execl("./laryngologist", "laryngologist", NULL);
        cleanup();
        report_error("[director.c] execl laryngologist", 1);
    }

    // SURGEON
    g_pids[7] = fork();
    if(g_pids[7] == -1) {
        cleanup();
        report_error("[director.c] fork surgeon", 1); }
    if(g_pids[7] == 0) {
        execl("./surgeon", "surgeon", NULL);
        cleanup();
        report_error("[director.c] execl surgeon", 1);
    }

    // PEDIATRICIAN
    g_pids[8] = fork();
    if(g_pids[8] == -1) {
        cleanup();
        report_error("[director.c] fork pediatrician", 1); 
    }
    if(g_pids[8] == 0) {
        execl("./pediatrician", "pediatrician", NULL);
        report_error("[director.c] execl pediatrician", 1);
    }

    // PC DOCTOR
    g_pids[9] = fork();
    if(g_pids[9] == -1) {
        cleanup();
        report_error("[director.c] fork pc_doctor", 1); }
    if(g_pids[9] == 0) {
        execl("./pc_doctor", "pc_doctor", NULL);
        cleanup();
        report_error("[director.c] execl pc_doctor", 1);
    }

    return 0;
}

void print_final_report() {
    struct PatientStats *final_stats = shmat(g_shm_stats, NULL, 0);
    if(final_stats == (void *)-1) {
        LOG_PRINTF("[ERROR] Cannot read final statistics");
        return;
    }

    int specialist_total = final_stats->cardiologist_count + final_stats->neurologist_count + final_stats->eye_doctor_count + final_stats->laryngologist_count + final_stats->surgeon_count + final_stats->pediatrician_count;

    int pc_doctor_count = final_stats->pc_doctor_count;
    int sent_home = final_stats->sent_home;
    int total_patients = final_stats->total_patients;
    
    int sent_to_specialists = pc_doctor_count - sent_home;
    
    if(total_patients < pc_doctor_count) {
        total_patients = pc_doctor_count;
    }

    LOG_PRINTF("\n========== FINAL REPORT ==========");
    LOG_PRINTF("Total patients generated: %d", total_patients);
    LOG_PRINTF("Patients treated by PC Doctor: %d", pc_doctor_count);
    LOG_PRINTF("Patients sent home (by PC Doctor): %d", sent_home);
    LOG_PRINTF("Patients referred to specialists: %d", sent_to_specialists);
    LOG_PRINTF("Patients treated by specialists: %d", specialist_total);
    LOG_PRINTF("  - Cardiologist: %d", final_stats->cardiologist_count);
    LOG_PRINTF("  - Neurologist: %d", final_stats->neurologist_count);
    LOG_PRINTF("  - Eye Doctor: %d", final_stats->eye_doctor_count);
    LOG_PRINTF("  - Laryngologist: %d", final_stats->laryngologist_count);
    LOG_PRINTF("  - Surgeon: %d", final_stats->surgeon_count);
    LOG_PRINTF("  - Pediatrician: %d", final_stats->pediatrician_count);
    
    int waiting_for_pc = total_patients - pc_doctor_count;
    int waiting_for_specialist = sent_to_specialists - specialist_total;
    
    if(waiting_for_pc > 0 || waiting_for_specialist > 0) {
        LOG_PRINTF("--- During evacuation ---");
        if(waiting_for_pc > 0) LOG_PRINTF("Waiting for PC Doctor: %d", waiting_for_pc);
        if(waiting_for_specialist > 0) LOG_PRINTF("Waiting for specialist: %d", waiting_for_specialist);
    }
    LOG_PRINTF("==================================\n");
    
    if(shmdt(final_stats) == -1) report_error("[director.c] shmdt final_stats", 0);
}

void manage_registration(int N, int patients_in_queue) {
    if(patients_in_queue >= N/2 && g_pids[1] <= 0) {
        LOG_PRINTF("|DIRECTOR| Queue too long (%d) - opening 2nd registration", patients_in_queue);
        g_pids[1] = fork();
        if(g_pids[1] == -1) {
            cleanup();
            report_error("[director.c] fork 2nd registration", 1);
        }
        if(g_pids[1] == 0) {
            execl("./registration", "registration", NULL);
            cleanup();
            report_error("[director.c] execl registration 2", 1);
        }
    }
    else if(patients_in_queue < N/3 && g_pids[1] > 0) {
        LOG_PRINTF("|DIRECTOR| Queue short (%d) - closing 2nd registration", patients_in_queue);
        kill(g_pids[1], SIGUSR2);
        waitpid(g_pids[1], NULL, 0);
        g_pids[1] = 0;
    }
}

void check_and_remove_from_specialist_queue(int queue_id, int specialist_index, const char *specialist_name) {
    struct msqid_ds queue_stat;
    
    if(msgctl(queue_id, IPC_STAT, &queue_stat) == -1) {
        report_error("[director.c] msgctl_queue_stat", 1);
    }
    
    if(queue_stat.msg_qnum > 0 && g_pids[specialist_index] > 0) {
        pid_t pid = g_pids[specialist_index];
        char status_path[64];
        snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
        
        FILE *status_file = fopen(status_path, "r");
        int is_suspended = 0;
        
        if(status_file == NULL) {
            is_suspended = 1;
        } else {
            char line[256];
            while(fgets(line, sizeof(line), status_file) != NULL) {
                if(strncmp(line, "State:", 6) == 0) {
                    if(strchr(line, 'T') != NULL) {
                        is_suspended = 1;
                    }
                    break;
                }
            }
            fclose(status_file);
        }
        
        if(!is_suspended) return;
        
        if(status_file == NULL) {
            LOG_PRINTF("|DIRECTOR| %s (PID %d) is dead, removing patients from queue",
                       specialist_name, pid);
        } else {
            LOG_PRINTF("|DIRECTOR| %s (PID %d) is suspended, removing patients from queue",
                       specialist_name, pid);
        }
        
        struct PatientCard msg;
        int removed_count = 0;
        while(msgrcv(queue_id, &msg, sizeof(struct PatientCard) - sizeof(long), -3, IPC_NOWAIT) != -1) {
            removed_count++;
            LOG_PRINTF("|DIRECTOR| Removed patient %d from %s queue", msg.patient_id, specialist_name);
            
            msg.mtype = msg.patient_id;
            msg.sdoc_dec = -1;
            
            if(msgsnd(queue_id, &msg, sizeof(struct PatientCard) - sizeof(long), IPC_NOWAIT) == -1) {
                if(errno == EAGAIN || errno == EWOULDBLOCK) {
                    if(msgsnd(queue_id, &msg, sizeof(struct PatientCard) - sizeof(long), 0) == -1) {
                        if(errno != EINTR) {
                            LOG_PRINTF("|DIRECTOR| Failed to send rejection to patient %d", msg.patient_id);
                        }
                    }
                } else if(errno != EINTR) {
                    LOG_PRINTF("|DIRECTOR| Error sending rejection to patient %d: %d", msg.patient_id, errno);
                }
            }
        }
        
        if(removed_count > 0) {
            LOG_PRINTF("|DIRECTOR| Removed total %d patients from %s queue", removed_count, specialist_name);
        }
    }
}

void send_doctor_to_ward() {
    int sdoc = (rand() % 6) + 3;
    if(g_pids[sdoc] > 0) {
        LOG_PRINTF("|DIRECTOR| Sending doctor (PID %d) to ward", g_pids[sdoc]);
        kill(g_pids[sdoc], SIGUSR1);
    }
}

int main() {

    atexit(cleanup_ipc);
    
    srand(time(NULL));

    printf("|DIRECTOR| Opening ER...\n");
    printf("Enter N (waiting room size): ");
    
    int N;
    if(scanf("%d", &N) != 1 || N <= 0 || N > 30001) {
        report_error("Invalid N value", 1);
    }

    for(int i = 0; i < 10; i++) g_pids[i] = 0;

    struct sigaction sa_int, sa_chld;
    
    sa_int.sa_handler = evacuation;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if(sigaction(SIGINT, &sa_int, NULL) == -1) report_error("[director.c] sigaction SIGINT", 1);
    
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if(sigaction(SIGCHLD, &sa_chld, NULL) == -1) report_error("[director.c] sigaction SIGCHLD", 1);

    if(init_shared_memory() == -1){
        cleanup_ipc();
        report_error("[director.c] init_shared_memory", 1);
    }
    if(init_semaphores(N) == -1){
        cleanup_ipc();
        report_error("[director.c] init_semaphores", 1); 
    }
    if(init_message_queues() == -1) { 
        cleanup_ipc();
        report_error("[director.c] init_message_queues", 1); 
    }

    int log_fd = creat(LOG_FILE, 0666);
    if(log_fd == -1) {
        cleanup_ipc(); 
        report_error("[director.c] creat log file", 1); 
    }
    close(log_fd);

    if(start_staff() == -1) {
        cleanup();
        report_error("[director.c] start_staff", 1); 
    }

    LOG_PRINTF("|DIRECTOR| ER opened with N=%d", N);

    struct msqid_ds queue_stat;
    time_t last_check = time(NULL);
    time_t last_doctor_check = time(NULL);

    while(is_ER_open==1) {
        if(!is_ER_open) break;
        
        if(msgctl(g_msg_pat_reg, IPC_STAT, &queue_stat) == -1) {
            if(errno == EINTR) continue;
            report_error("[director.c] msgctl queue stat", 0);
            continue;
        }

        if(!is_ER_open) break;

        manage_registration(N, queue_stat.msg_qnum);

        if(time(NULL) > last_doctor_check) {
            last_doctor_check = time(NULL);
            check_and_remove_from_specialist_queue(g_msg_pat_cardio, 3, "CARDIOLOGIST");
            check_and_remove_from_specialist_queue(g_msg_pat_neuro, 4, "NEUROLOGIST");
            check_and_remove_from_specialist_queue(g_msg_pat_eye, 5, "EYE DOCTOR");
            check_and_remove_from_specialist_queue(g_msg_pat_laryng, 6, "LARYNGOLOGIST");
            check_and_remove_from_specialist_queue(g_msg_pat_surgeon, 7, "SURGEON");
            check_and_remove_from_specialist_queue(g_msg_pat_pediatr, 8, "PEDIATRICIAN");
        }

        if(time(NULL) > last_check) {
            last_check = time(NULL);
            if((rand() % 100) < 5) {
                send_doctor_to_ward();
            }
        }
        
        if(!is_ER_open) break;
    }

    LOG_PRINTF("|DIRECTOR| EVACUATION!");
    print_final_report();
    cleanup();
    printf("|DIRECTOR| ER evacuated!\n");

    return 0;
}