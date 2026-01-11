#include "common.h"
#include "errors.h"
#include <signal.h>

volatile sig_atomic_t is_ER_open = 1;

pid_t pids[5];

void evacuation(){
    printf("[DIRECTOR] Evacuation of the emergency room!");
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
    if(scanf("%d", &N) != 1) report_error("[director.c] error: scanf",1);


    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, 1, 0600 | IPC_CREAT);
    if(semget_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);

    union semun arg;
    arg.val = N;
    int semctl_waiting_room = semctl(semget_waiting_room, 0 , SETVAL, arg);
    if(semctl_waiting_room == -1) report_error("[patient.c] error: semtcl_waiting_room", 1);

    //SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 1, 0600 | IPC_CREAT);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);
    

    //OPENING REGISTRATION
    pids[0] = fork();
    if(pids[0] == 0){
        execl("./registration", "registration", NULL);
        report_error("[director.c] error: reg=fork()", 1);
    }

    //HIRING PRIMARY CARE DOCTORS
    for(int i=0; i<3; i++){
        pids[1+i] = fork();
        if(pids[1+i] == 0){
            execl("./pc_doctor", "pc_doctor", NULL);
            report_error("[director.c] error: doc=fork()", 1);
        }
    }

    //PATIENT GENERATOR
    pids[4] = fork();
    if(pids[4] == 0){
        execl("./generator", "generator", NULL);
        report_error("[director.c] error: pat_reg = fork()",1);
    }

    printf("[DIRECTOR] ER opened!\n");

    while(is_ER_open){
        sleep(1);
    }


    //CLEANING
    printf("[DIRECTOR] Closing and cleaning ER...\n");

    for(int i=0; i<5; i++){
        if(pids[i] > 0) {
            kill(pids[i], SIGTERM);
        }
    }

    //WAITING UNTIL PROCESSES ARE KILLED
    while(wait(NULL) > 0){
        sleep(1);
    }
    
    //DELETING MESSAGE QUEUE
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    int id_msg_pat_reg = msgget(key_msg_pat_reg, 0600);
    int msgctl_del_pat_reg = msgctl(id_msg_pat_reg, IPC_RMID, NULL);
    if(msgctl_del_pat_reg == -1) report_error("[director.c] error: msgctl_del_pat_reg", 1);

    //DELETING WAITING ROOM SEMAPHORE
    int semctl_del_waiting_room = semctl(semget_waiting_room, 0 , IPC_RMID, arg);
    if(semctl_del_waiting_room == -1) report_error("[director.c] error: semtcl_del_waiting_room", 1);
    return 0;

    //DELETING DOCTOR SEMAPHORE
    int semctl_del_doc = semctl(semget_doc, 0 , IPC_RMID, arg);
    if(semctl_del_doc == -1) report_error("[director.c] error: semtcl_del_doc", 1);
    return 0;
}

