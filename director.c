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

    //2 PIDS FOR REGISTRATION, 1 PID FOR GENERATOR, 1 PER SPECIALIZED DOCTOR (THERE ARE 6 OF THEM) , REST FOR PC DOCTORS
    pid_t pids[9+doctors];

    pids[1] = 0;

    //SHARED MEMORY REGISTRATION->DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    int shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard), 0600 | IPC_CREAT);

    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[director.c] error: key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, 1, 0600 | IPC_CREAT);
    if(semget_waiting_room == -1) report_error("[director.c] error: key_sem_waiting_room", 1);

    union semun sem;
    sem.val = N;
    int semctl_waiting_room = semctl(semget_waiting_room, 0 , SETVAL, sem);
    if(semctl_waiting_room == -1) report_error("[director.c] error: semtcl_waiting_room", 1);

    //SEMAPHORE REGISTRATION->DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600 | IPC_CREAT);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);
    
    sem.val = 1;
    int semctl_card_reg_doc = semctl(semget_doc, 0 , SETVAL, sem);
    if(semctl_card_reg_doc == -1) report_error("[pc_doctor.c] error: semtcl_card_doc", 1);

    sem.val = 0;
    int semctl_card_doc_reg = semctl(semget_doc, 1 , SETVAL, sem);
    if(semctl_card_doc_reg == -1) report_error("[pc_doctor.c] error: semtcl_card_doc", 1);


    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[director.c] error: key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[director.c] error: msg_pat_reg", 1);

    //MESSAGE QUEUE DOCTOR<->PATIENT
    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[director.c] key_msg_doc_pat", 1);

    int msg_doc_pat = msgget(key_msg_doc_pat, 0600 | IPC_CREAT);
    if(msg_doc_pat == -1) report_error("[director.c] msg_doc_pat", 1);

    //MESSAGE QUEUE PATIENT->CARDIOLOGIST
    key_t key_msg_pat_cardio = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio == -1) report_error("[director.c] error: key_msg_pat_cardio", 1);

    int msg_pat_cardio = msgget(key_msg_pat_cardio, 0600 | IPC_CREAT);
    if(msg_pat_cardio == -1) report_error("[director.c] error: msg_pat_cardio", 1);

    //MESSAGE QUEUE PATIENT->NEUROLOGIST
    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[director.c] error: key_msg_pat_neuro", 1);

    int msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(msg_pat_neuro == -1) report_error("[director.c] error: msg_pat_neuro", 1);

    //MESSAGE QUEUE PATIENT->EYE DOC
    key_t key_msg_pat_eye = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eye == -1) report_error("[director.c] error: key_msg_pat_eye", 1);

    int msg_pat_eye = msgget(key_msg_pat_eye, 0600 | IPC_CREAT);
    if(msg_pat_eye == -1) report_error("[director.c] error: msg_pat_eye", 1);

    //MESSAGE QUEUE PATIENT->LARYNGOLOGIST
    key_t key_msg_pat_laryng = ftok(FTOK_PATH, ID_MSG_PAT_LARYNG);
    if(key_msg_pat_laryng == -1) report_error("[director.c] error: key_msg_pat_laryng", 1);

    int msg_pat_laryng = msgget(key_msg_pat_laryng, 0600 | IPC_CREAT);
    if(msg_pat_laryng == -1) report_error("[director.c] error: msg_pat_laryng", 1);

    //MESSAGE QUEUE PATIENT->SURGEON
    key_t key_msg_pat_surgeon = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon == -1) report_error("[director.c] error: key_msg_pat_surgeon", 1);

    int msg_pat_surgeon = msgget(key_msg_pat_surgeon, 0600 | IPC_CREAT);
    if(msg_pat_surgeon == -1) report_error("[director.c] error: msg_pat_surgeon", 1);

    //MESSAGE QUEUE PATIENT->PEDATRICIAN
    key_t key_msg_pat_pedatr = ftok(FTOK_PATH, ID_MSG_PAT_PEDATR);
    if(key_msg_pat_pedatr == -1) report_error("[director.c] error: key_msg_pat_pedatr", 1);

    int msg_pat_pedatr = msgget(key_msg_pat_pedatr, 0600 | IPC_CREAT);
    if(msg_pat_pedatr == -1) report_error("[director.c] error: msg_pat_pedatr", 1);




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
        report_error("[director.c] error: generator = fork()",1);
    }

    //HIRING SPECIALIZED DOCTORS
    pids[3] = fork();
    if(pids[3] == 0){
        execl("./cardiologist", "cardiologist", NULL);
        report_error("[director.c] error: cardiologist = fork()",1);
    }

    pids[4] = fork();
    if(pids[4] == 0){
        execl("./neurologist", "neurologist", NULL);
        report_error("[director.c] error: neurologist = fork()",1);
    }

    pids[5] = fork();
    if(pids[5] == 0){
        execl("./eyedoc", "eyedoc", NULL);
        report_error("[director.c] error: eyedoc = fork()",1);
    }
    
    pids[6] = fork();
    if(pids[6] == 0){
        execl("./laryngologist", "laryngologist", NULL);
        report_error("[director.c] error: laryngologist = fork()",1);
    }

    pids[7] = fork();
    if(pids[7] == 0){
        execl("./surgeon", "surgeon", NULL);
        report_error("[director.c] error: surgeon = fork()",1);
    }

    pids[8] = fork();
    if(pids[8] == 0){
        execl("./pedatrician", "pedatrician", NULL);
        report_error("[director.c] error: pedatrician = fork()",1);
    }


    //HIRING PRIMARY CARE DOCTORS
    for(int i=0; i<doctors; i++){
        pids[9+i] = fork();
        if(pids[9+i] == 0){
            execl("./pc_doctor", "pc_doctor", NULL);
            report_error("[director.c] error: doc=fork()", 1);
        }
    }

    


    struct msqid_ds waiting_room_stat;
    printf("|DIRECTOR| ER opened!\n");

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

    for(int i=0; i < 9 + doctors; i++){ 
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

    //DELETING MESSAGE QUEUE PATIENT<->CARDIOLOGIST
    int msgctl_del_pat_cardio = msgctl(msg_pat_cardio, IPC_RMID, NULL);
    if(msgctl_del_pat_cardio == -1) report_error("[director.c] error: msgctl_del_pat_cardio", 1);

    //DELETING MESSAGE QUEUE PATIENT<->NEUROLOGIST
    int msgctl_del_pat_neuro = msgctl(msg_pat_neuro, IPC_RMID, NULL);
    if(msgctl_del_pat_neuro == -1) report_error("[director.c] error: msgctl_del_pat_neuro", 1);

    //DELETING MESSAGE QUEUE PATIENT<->EYE DOCTOR
    int msgctl_del_pat_doc_eye = msgctl(msg_pat_eye, IPC_RMID, NULL);
    if(msgctl_del_pat_doc_eye == -1) report_error("[director.c] error: msgctl_del_pat_doc_eye", 1);

    //DELETING MESSAGE QUEUE PATIENT<->LARYNGOLOGIST
    int msgctl_del_pat_laryng = msgctl(msg_pat_laryng, IPC_RMID, NULL);
    if(msgctl_del_pat_laryng == -1) report_error("[director.c] error: msgctl_del_pat_laryng", 1);

    //DELETING MESSAGE QUEUE PATIENT<->SURGEON
    int msgctl_del_pat_surgeon = msgctl(msg_pat_surgeon, IPC_RMID, NULL);
    if(msgctl_del_pat_surgeon == -1) report_error("[director.c] error: msgctl_del_pat_surgeon", 1);

    //DELETING MESSAGE QUEUE PATIENT<->PEDATRICIAN
    int msgctl_del_pat_pedatr = msgctl(msg_pat_pedatr, IPC_RMID, NULL);
    if(msgctl_del_pat_pedatr == -1) report_error("[director.c] error: msgctl_del_pat_pedatr", 1);
    
    printf("[DIRECTOR] ER evacuated!\n");

    return 0;
}

