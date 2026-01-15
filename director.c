#include "common.h"
#include "errors.h"
#include <signal.h>

volatile sig_atomic_t is_ER_open = 1;


void evacuation(){
    printf("[DIRECTOR] Evacuation of the emergency room!\n");
    is_ER_open = 0;
                //KILLING PROCESSES
}

int main(){
    //CTRL+C = EVACUATION OF ER
    signal(SIGINT, evacuation);
    
    printf("[DIRECTOR] Opening ER...\n");

    
    //SETTING N VALUE
    printf("Please enter N value (size of the waiting room):");
    int N;
    if(scanf("%d", &N) != 1) report_error("[director.c] error: scanf (N)",1);

    printf("How many doctors need to be hired?");
    int doctors;
    if(scanf("%d", &doctors) != 1) report_error("[director.c] error: scanf (doctors)",1);

    //2 PIDS FOR REGISTRATION, 1 PID FOR GENERATOR, REST FOR DOCTORS
    pid_t pids[3+doctors];

    pids[1] = 0;

    //SHARED MEMORY REGISTRATION->DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    int shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard) - sizeof(long), 0600 | IPC_CREAT);

    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[director.c] error: key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, 1, 0600 | IPC_CREAT);
    if(semget_waiting_room == -1) report_error("[director.c] error: key_sem_waiting_room", 1);

    union semun arg;
    arg.val = N;
    int semctl_waiting_room = semctl(semget_waiting_room, 0 , SETVAL, arg);
    if(semctl_waiting_room == -1) report_error("[director.c] error: semtcl_waiting_room", 1);

    //SEMAPHORE REGISTRATION->DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600 | IPC_CREAT);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);
    
    arg.val = 1;
    int semctl_doc_empty = semctl(semget_doc, 0 , SETVAL, arg);
    if(semctl_doc_empty == -1) report_error("[pc_doctor.c] error: semtcl_doc (empty)", 1);

    arg.val = 0;
    int semctl_doc_full = semctl(semget_doc, 1 , SETVAL, arg);
    if(semctl_doc_full == -1) report_error("[pc_doctor.c] error: semtcl_doc (full)", 1);


    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[director.c] error: key_shm_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[director.c] error: shmget_pat_reg", 1);

    //MESSAGE QUEUE DOCTOR<->PATIENT
    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[director.c] key_msg_doc_pat", 1);

    int msg_doc_pat = msgget(key_msg_doc_pat, 0600 | IPC_CREAT);
    if(msg_doc_pat == -1) report_error("[director.c] msg_doc_pat", 1);




    //OPENING REGISTRATION
    pids[0] = fork();
    if(pids[0] == 0){
        execl("./registration", "registration", NULL);
        report_error("[director.c] error: reg=fork()", 1);
    }

    //PATIENT GENERATOR
    pids[2] = fork();
    if(pids[2] == 0){
        execl("./generator", "generator", NULL);
        report_error("[director.c] error: pat_reg = fork()",1);
    }

    //HIRING PRIMARY CARE DOCTORS
    for(int i=0; i<doctors; i++){
        pids[3+i] = fork();
        if(pids[3+i] == 0){
            execl("./pc_doctor", "pc_doctor", NULL);
            report_error("[director.c] error: doc=fork()", 1);
        }
    }

    
    struct msqid_ds waiting_room_stat;
    printf("[DIRECTOR] ER opened!\n");

    while(is_ER_open){
        int msgctl_waiting_room_stat = msgctl(msg_pat_reg, IPC_STAT, &waiting_room_stat);
        if(msgctl_waiting_room_stat == -1) report_error("[director.c] msgctl_waiting_room_stat", 1);

        int patients_in_queue = waiting_room_stat.msg_qnum;
        if(patients_in_queue >= N/2 && pids[1] == 0){
            printf("|DIRECTOR| Queue is too long - opening second registration...\n");
            pids[1] = fork();
            if(pids[1] == 0){
                execl("./registration", "registration", NULL);
                report_error("[director.c] reg=fork()",1);
            }
        }
        else if(patients_in_queue<(N/3) && pids[1] > 0){
            printf("|DIRECTOR| Queue is too short - closing second registration...\n");
            kill(pids[1], SIGTERM);
            waitpid(pids[1], NULL, 0);              //PROHIBITED OR NOT??
            pids[1] = 0;
        }

        //sleep(1);
    }


    //CLEANING

    for(int i=0; i < 3 + doctors; i++){ 
        if(pids[i] > 0) {
            kill(pids[i], SIGTERM);
    }
}

    //WAITING UNTIL PROCESSES ARE KILLED
    while(wait(NULL) > 0){
        //sleep(1);
    }
    
    //DELETING MESSAGE QUEUE PATIENT<->REGISTRATION
    int msgctl_del_pat_reg = msgctl(msg_pat_reg, IPC_RMID, NULL);
    if(msgctl_del_pat_reg == -1) report_error("[director.c] error: msgctl_del_pat_reg", 1);

    //DELETING MESSAGE QUEUE DOCTOR<->PATIENT
    int msgctl_del_doc_pat = msgctl(msg_doc_pat, IPC_RMID, NULL);
    if(msgctl_del_doc_pat == -1) report_error("[director.c] error: msgctl_del_doc_pat", 1);

    //DELETING WAITING ROOM SEMAPHORE
    int semctl_del_waiting_room = semctl(semget_waiting_room, 0, IPC_RMID, NULL);
    if(semctl_del_waiting_room == -1) report_error("[director.c] error: semtcl_del_waiting_room", 1);
  
    //DELETING SHARED MEMORY REGISTRATION->DOC
    int shmtcl_reg_doc_del = shmctl(shmget_reg_doc, IPC_RMID, NULL);
    if(shmtcl_reg_doc_del == -1) report_error("[director.c] error: shmtcl_del_reg_doc", 1);

    //DELETING REGISTRATION->DOCTOR SEMAPHORE
    int semctl_del_doc = semctl(semget_doc, 0 , IPC_RMID, NULL);
    if(semctl_del_doc == -1) report_error("[director.c] error: semtcl_del_doc", 1);

    
    printf("[DIRECTOR] ER evacuated!\n");

    return 0;
}

