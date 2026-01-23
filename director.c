#include "common.h"
#include "errors.h"
#include <signal.h>

volatile int is_ER_open = 1;

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

void cleanup() {
   
    for(int i = 0; i < 10; i++) {
        if(g_pids[i] > 0) {
            kill(g_pids[i], SIGTERM);
        }
    }
    
    kill(0, SIGTERM);
    
    while(wait(NULL) > 0) {}
    if(g_shmget_reg_doc >= 0) shmctl(g_shmget_reg_doc, IPC_RMID, NULL);
    if(g_semget_waiting_room >= 0) semctl(g_semget_waiting_room, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_reg >= 0) semctl(g_semget_msg_pat_reg, 0, IPC_RMID, NULL);
    if(g_semget_doc >= 0) semctl(g_semget_doc, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_doc >= 0) semctl(g_semget_msg_pat_doc, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_cardio >= 0) semctl(g_semget_msg_pat_cardio, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_eyedoc >= 0) semctl(g_semget_msg_pat_eyedoc, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_laryng >= 0) semctl(g_semget_msg_pat_laryng, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_neuro >= 0) semctl(g_semget_msg_pat_neuro, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_pediatr >= 0) semctl(g_semget_msg_pat_pediatr, 0, IPC_RMID, NULL);
    if(g_semget_msg_pat_surgeon >= 0) semctl(g_semget_msg_pat_surgeon, 0, IPC_RMID, NULL);
    if(g_semget_gen >= 0) semctl(g_semget_gen, 0, IPC_RMID, NULL);
    if(g_semget_log >= 0) semctl(g_semget_log, 0, IPC_RMID, NULL);
    if(g_shm_stats >= 0) shmctl(g_shm_stats, IPC_RMID, NULL);
    if(g_semget_stats >= 0) semctl(g_semget_stats, 0, IPC_RMID, NULL);
    if(g_msg_pat_reg >= 0) msgctl(g_msg_pat_reg, IPC_RMID, NULL);
    if(g_msg_doc_pat >= 0) msgctl(g_msg_doc_pat, IPC_RMID, NULL);
    if(g_msg_pat_cardio >= 0) msgctl(g_msg_pat_cardio, IPC_RMID, NULL);
    if(g_msg_pat_neuro >= 0) msgctl(g_msg_pat_neuro, IPC_RMID, NULL);
    if(g_msg_pat_eye >= 0) msgctl(g_msg_pat_eye, IPC_RMID, NULL);
    if(g_msg_pat_laryng >= 0) msgctl(g_msg_pat_laryng, IPC_RMID, NULL);
    if(g_msg_pat_surgeon >= 0) msgctl(g_msg_pat_surgeon, IPC_RMID, NULL);
    if(g_msg_pat_pediatr >= 0) msgctl(g_msg_pat_pediatr, IPC_RMID, NULL);
}

void evacuation(){
    LOG_PRINTF("[DIRECTOR] Evacuation of the emergency room!");
    is_ER_open = 0;
}

int main(){
    setbuf(stdout, NULL);
    srand(time(NULL));

    srand(time(NULL));

    printf("[DIRECTOR] Opening ER...\n");

    
    //SETTING N VALUE
    printf("Please enter N value (size of the waiting room):\n");
    int N;
    if(scanf("%d", &N) != 1 || N <= 0) report_error("Invalid N value", 1);


    //2 PIDS FOR REGISTRATION, 1 PID FOR GENERATOR, 1 PER SPECIALIZED DOCTOR (THERE ARE 6 OF THEM) , 1 PID FOR PC DOCTOR
    for(int i = 0;i<10;i++){
        g_pids[i] = 0;
    }

    //VARIABLES FOR STATS (DECLARE AT BEGINNING SO AVAILABLE AT END)
    int shm_stats;
    int semget_stats;

    //CTRL+C = EVACUATION OF ER
    signal(SIGINT, evacuation);

    //SHARED MEMORY REGISTRATION<->DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[director.c] key_shm_reg_doc", 1);

    g_shmget_reg_doc = shmget(key_shm_reg_doc, sizeof(struct PatientCard), 0600 | IPC_CREAT);
    if(g_shmget_reg_doc == -1 ) report_error("[director.c] shmget_reg_doc", 1);


    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[director.c] key_sem_waiting_room", 1);

    g_semget_waiting_room = semget(key_sem_waiting_room, 1, 0600 | IPC_CREAT);
    if(g_semget_waiting_room == -1) report_error("[director.c] semget_waiting_room", 1);

    union semun sem;
    sem.val = N;

    int semctl_waiting_room = semctl(g_semget_waiting_room, 0 , SETVAL, sem);
    if(semctl_waiting_room == -1) report_error("[director.c] semtcl_waiting_room", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[director.c] key_sem_msg_pat_reg", 1);

    g_semget_msg_pat_reg = semget(key_sem_msg_pat_reg, 1, 0600 | IPC_CREAT); 
    if(g_semget_msg_pat_reg == -1) report_error("[director.c] semget_msg_pat_reg", 1);

    sem.val = 1020; //  16384:16=1024 (1020 set for safety), 1024 = sizeof(MsgBUff)
    int semctl_msg_pat_reg = semctl(g_semget_msg_pat_reg, 0, SETVAL, sem);
    if(semctl_msg_pat_reg == -1) report_error("[director.c] semctl_msg_pat_reg",1);


    //SEMAPHORE REGISTRATION<->DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[director.c] key_sem_doc", 1);

    g_semget_doc = semget(key_sem_doc, 2, 0600 | IPC_CREAT);
    if(g_semget_doc == -1) report_error("[director.c] semget_doc", 1);
    
    sem.val = 1;
    int semctl_card_reg_doc = semctl(g_semget_doc, 0 , SETVAL, sem);
    if(semctl_card_reg_doc == -1) report_error("[director.c] semtcl_card_reg_doc", 1);

    sem.val = 0;
    int semctl_card_doc_reg = semctl(g_semget_doc, 1 , SETVAL, sem);
    if(semctl_card_doc_reg == -1) report_error("[director.c] semtcl_card_doc_reg", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->PC DOCTOR
    key_t key_sem_msg_pat_doc = ftok(FTOK_PATH, ID_SEM_MSG_PAT_DOC);
    if(key_sem_msg_pat_doc == -1) report_error("[patient.c] key_sem_msg_pat_doc", 1);

    g_semget_msg_pat_doc = semget(key_sem_msg_pat_doc, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_doc == -1) report_error("[director.c] semget_msg_pat_doc", 1);

    sem.val = 250;
    int semctl_msg_pat_doc = semctl(g_semget_msg_pat_doc, 0, SETVAL, sem);
    if(semctl_msg_pat_doc == -1) report_error("[director.c] semctl_msg_pat_doc", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->CARDIOLOGIST
    key_t key_sem_msg_pat_cardio = ftok(FTOK_PATH, ID_SEM_MSG_CARDIO);
    if(key_sem_msg_pat_cardio == -1) report_error("[director.c] key_sem_msg_pat_cardio", 1);

    g_semget_msg_pat_cardio = semget(key_sem_msg_pat_cardio, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_cardio == -1) report_error("[director.c] semget_msg_pat_cardio", 1);

    
    int semctl_msg_pat_cardio = semctl(g_semget_msg_pat_cardio, 0, SETVAL, sem);
    if(semctl_msg_pat_cardio == -1) report_error("[director.c] semctl_msg_pat_cardio",1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->EYE DOCTOR
    key_t key_sem_msg_pat_eyedoc = ftok(FTOK_PATH, ID_SEM_MSG_EYEDOC);
    if(key_sem_msg_pat_eyedoc == -1) report_error("[director.c] key_sem_msg_pat_eyedoc", 1);

    g_semget_msg_pat_eyedoc = semget(key_sem_msg_pat_eyedoc, 1, 0600 | IPC_CREAT); 
    if(g_semget_msg_pat_eyedoc == -1) report_error("[director.c] semget_msg_pat_eyedoc", 1);

    int semctl_msg_pat_eyedoc = semctl(g_semget_msg_pat_eyedoc, 0, SETVAL, sem);
    if(semctl_msg_pat_eyedoc == -1) report_error("[director.c] semctl_msg_pat_eyedoc",1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->LARYNGOLOGIST
    key_t key_sem_msg_pat_laryng = ftok(FTOK_PATH, ID_SEM_MSG_LARYNG);
    if(key_sem_msg_pat_laryng == -1) report_error("[director.c] key_sem_msg_pat_laryng", 1);

    g_semget_msg_pat_laryng = semget(key_sem_msg_pat_laryng, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_laryng == -1) report_error("[director.c] semget_msg_pat_laryng", 1);

    int semctl_msg_pat_laryng = semctl(g_semget_msg_pat_laryng, 0, SETVAL, sem);
    if(semctl_msg_pat_laryng == -1) report_error("[director.c] semctl_msg_pat_laryng",1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->NEUROLOGIST
    key_t key_sem_msg_pat_neuro = ftok(FTOK_PATH, ID_SEM_MSG_NEURO);
    if(key_sem_msg_pat_neuro == -1) report_error("[director.c] key_sem_msg_pat_neuro", 1);

    g_semget_msg_pat_neuro = semget(key_sem_msg_pat_neuro, 1, 0600 | IPC_CREAT);
    if(g_semget_msg_pat_neuro == -1) report_error("[director.c] semget_msg_pat_neuro", 1);

    int semctl_msg_pat_neuro = semctl(g_semget_msg_pat_neuro, 0, SETVAL, sem);
    if(semctl_msg_pat_neuro == -1) report_error("[director.c] semctl_msg_pat_neuro",1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->PEDIATRICIAN
    key_t key_sem_msg_pat_pediatr = ftok(FTOK_PATH, ID_SEM_MSG_PEDIATR);
    if(key_sem_msg_pat_pediatr == -1) report_error("[director.c] key_sem_msg_pat_pediatr", 1);

    g_semget_msg_pat_pediatr = semget(key_sem_msg_pat_pediatr, 1, 0600 | IPC_CREAT); 
    if(g_semget_msg_pat_pediatr == -1) report_error("[director.c] semget_msg_pat_pediatr", 1);

    int semctl_msg_pat_pediatr = semctl(g_semget_msg_pat_pediatr, 0, SETVAL, sem);
    if(semctl_msg_pat_pediatr == -1) report_error("[director.c] semctl_msg_pat_pediatr",1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->SURGEON
    key_t key_sem_msg_pat_surgeon = ftok(FTOK_PATH, ID_SEM_MSG_SURGEON);
    if(key_sem_msg_pat_surgeon == -1) report_error("[director.c] key_sem_msg_pat_surgeon", 1);

    g_semget_msg_pat_surgeon = semget(key_sem_msg_pat_surgeon, 1, 0600 | IPC_CREAT); 
    if(g_semget_msg_pat_surgeon == -1) report_error("[director.c] semget_msg_pat_surgeon", 1);

    int semctl_msg_pat_surgeon = semctl(g_semget_msg_pat_surgeon, 0, SETVAL, sem);
    if(semctl_msg_pat_surgeon == -1) report_error("[director.c] semctl_msg_pat_surgeon",1);


    //SEMAPHORE GENERATOR
    key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen == -1) report_error("[director.c] key_sem_gen", 1);

    g_semget_gen = semget(key_sem_gen, 1, 0600 | IPC_CREAT);
    if(g_semget_gen == -1) report_error("[director.c] semget_gen", 1);

    sem.val = 20000;
    int semctl_gen = semctl(g_semget_gen, 0, SETVAL, sem);
    if(semctl_gen == -1) report_error("[director.c] semctl_gen",1);


    //SEMAPHORE LOG FILE
    key_t key_sem_log = ftok(FTOK_PATH, ID_SEM_LOG_FILE);
    if(key_sem_log == -1) report_error("[director.c] key_sem_log", 1);

    g_semget_log = semget(key_sem_log, 1, 0600 | IPC_CREAT);
    if(g_semget_log == -1) report_error("[director.c] semget_log", 1);

    sem.val = 1;
    int semctl_log = semctl(g_semget_log, 0, SETVAL, sem);
    if(semctl_log == -1) report_error("[director.c] semctl_log", 1);


    //SHARED MEMORY FOR STATS
    key_t key_shm_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_shm_stats == -1) report_error("[director.c] key_shm_stats", 1);

    g_shm_stats = shmget(key_shm_stats, sizeof(struct PatientStats), 0600 | IPC_CREAT);
    if(g_shm_stats == -1) report_error("[director.c] shm_stats", 1);

    struct PatientStats *stats = shmat(g_shm_stats, NULL, 0);
    if(stats == (void *)-1) report_error("[director.c] shmat stats", 1);
    
    memset(stats, 0, sizeof(struct PatientStats));
    shmdt(stats);


    //SEMAPHORE FOR STATS
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) report_error("[director.c] key_sem_stats", 1);

    g_semget_stats = semget(key_sem_stats, 1, 0600 | IPC_CREAT);
    if(g_semget_stats == -1) report_error("[director.c] semget_stats", 1);

    sem.val = 1;
    int semctl_stats = semctl(g_semget_stats, 0, SETVAL, sem);
    if(semctl_stats == -1) report_error("[director.c] semctl_stats", 1);


    //CREATE LOG FILE
    int log_fd = creat(LOG_FILE, 0666);
    if(log_fd == -1) report_error("[director.c] creat log file", 1);
    close(log_fd);


    //MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[director.c] key_msg_pat_reg", 1);

    g_msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(g_msg_pat_reg == -1) report_error("[director.c] msg_pat_reg", 1);


    //MESSAGE QUEUE DOCTOR->PATIENT
    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[director.c] key_msg_doc_pat", 1);

    g_msg_doc_pat = msgget(key_msg_doc_pat, 0600 | IPC_CREAT);
    if(g_msg_doc_pat == -1) report_error("[director.c] msg_doc_pat", 1);


    //MESSAGE QUEUE PATIENT->CARDIOLOGIST
    key_t key_msg_pat_cardio = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio == -1) report_error("[director.c] key_msg_pat_cardio", 1);

    g_msg_pat_cardio = msgget(key_msg_pat_cardio, 0600 | IPC_CREAT);
    if(g_msg_pat_cardio == -1) report_error("[director.c] msg_pat_cardio", 1);


    //MESSAGE QUEUE PATIENT->NEUROLOGIST
    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[director.c] key_msg_pat_neuro", 1);

    g_msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(g_msg_pat_neuro == -1) report_error("[director.c] msg_pat_neuro", 1);


    //MESSAGE QUEUE PATIENT->EYE DOC
    key_t key_msg_pat_eye = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eye == -1) report_error("[director.c] key_msg_pat_eye", 1);

    g_msg_pat_eye = msgget(key_msg_pat_eye, 0600 | IPC_CREAT);
    if(g_msg_pat_eye == -1) report_error("[director.c] msg_pat_eye", 1);


    //MESSAGE QUEUE PATIENT->LARYNGOLOGIST
    key_t key_msg_pat_laryng = ftok(FTOK_PATH, ID_MSG_PAT_LARYNG);
    if(key_msg_pat_laryng == -1) report_error("[director.c] key_msg_pat_laryng", 1);

    g_msg_pat_laryng = msgget(key_msg_pat_laryng, 0600 | IPC_CREAT);
    if(g_msg_pat_laryng == -1) report_error("[director.c] msg_pat_laryng", 1);


    //MESSAGE QUEUE PATIENT->SURGEON
    key_t key_msg_pat_surgeon = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon == -1) report_error("[director.c] key_msg_pat_surgeon", 1);

    g_msg_pat_surgeon = msgget(key_msg_pat_surgeon, 0600 | IPC_CREAT);
    if(g_msg_pat_surgeon == -1) report_error("[director.c] msg_pat_surgeon", 1);


    //MESSAGE QUEUE PATIENT->PEDIATRICIAN
    key_t key_msg_pat_pediatr = ftok(FTOK_PATH, ID_MSG_PAT_PEDIATR);
    if(key_msg_pat_pediatr == -1) report_error("[director.c] key_msg_pat_pediatr", 1);

    g_msg_pat_pediatr = msgget(key_msg_pat_pediatr, 0600 | IPC_CREAT);
    if(g_msg_pat_pediatr == -1) report_error("[director.c] msg_pat_pediatr", 1);



    //OPENING REGISTRATION
    g_pids[0] = fork();
    if(g_pids[0] == -1){
        report_error("[director.c] g_pids[0]=fork()", 1);
    }
    else if(g_pids[0] == 0){
        execl("./registration", "registration", NULL);
        report_error("[director.c] execl() -> registration", 1);
    }
    //2ND REGISTRATION
    g_pids[1]=0;


    //PATIENT GENERATOR
    g_pids[2] = fork();
    if(g_pids[2] == -1){
        report_error("[director.c] g_pids[0]=fork()", 1);
    }
    else if(g_pids[2] == 0){
        execl("./generator", "generator", NULL);
        report_error("[director.c] execl() -> generator",1);
    }


    //HIRING SPECIALIZED DOCTORS
    g_pids[3] = fork();
    if(g_pids[3]== -1){
        report_error("[director.c] g_pids[2]=fork()", 1);
    }
    else if(g_pids[3] == 0){
        execl("./cardiologist", "cardiologist", NULL);
        report_error("[director.c] execl() -> cardiologist",1);
    }

    g_pids[4] = fork();
    if(g_pids[4] == -1){
        report_error("[director.c] g_pids[3]=fork()", 1);
    }
    else if(g_pids[4] == 0){
        execl("./neurologist", "neurologist", NULL);
        report_error("[director.c] execl() -> neurologist",1);
    }

    g_pids[5] = fork();
    if(g_pids[5] == -1){
        report_error("[director.c] g_pids[4]=fork()", 1);
    }
    else if(g_pids[5] == 0){
        execl("./eyedoc", "eyedoc", NULL);
        report_error("[director.c] execl() -> eyedoc",1);
    }
    
    g_pids[6] = fork();
    if(g_pids[6] == -1){
        report_error("[director.c] g_pids[5]=fork()", 1);
    }
    else if(g_pids[6] == 0){
        execl("./laryngologist", "laryngologist", NULL);
        report_error("[director.c] execl() -> laryngologist",1);
    }

    g_pids[7] = fork();
    if(g_pids[7] == -1){
        report_error("[director.c] g_pids[6]=fork()", 1);
    }
    else if(g_pids[7] == 0){
        execl("./surgeon", "surgeon", NULL);
        report_error("[director.c] execl() -> surgeon",1);
    }

    g_pids[8] = fork();
    if(g_pids[8]==-1){
        report_error("[director.c] g_pids[7]=fork()", 1);
    }
    else if(g_pids[8] == 0){
        execl("./pediatrician", "pediatrician", NULL);
        report_error("[director.c] execl() -> pediatrician",1);
    }


    //HIRING PRIMARY CARE DOCTOR
    g_pids[9] = fork();
    if(g_pids[9]==-1){
        report_error("[director.c] g_pids[8]=fork()", 1);
    }
    else if(g_pids[9] == 0){
        execl("./pc_doctor", "pc_doctor", NULL);
        report_error("[director.c] execl() -> pc_doctor",1);
    }

    


    struct msqid_ds waiting_room_stat;
    printf("|DIRECTOR| ER opened!\n");

    while(is_ER_open){
        int msgctl_waiting_room_stat = msgctl(g_msg_pat_reg, IPC_STAT, &waiting_room_stat);
        if(msgctl_waiting_room_stat == -1) report_error("[director.c] msgctl_waiting_room_stat", 1);

        int patients_in_queue = waiting_room_stat.msg_qnum;
        if(patients_in_queue >= N/2 && g_pids[1] == 0){
            LOG_PRINTF("|DIRECTOR| Queue is too long - opening second registration...");
            g_pids[1] = fork();
            if(g_pids[1] == 0){
                execl("./registration", "registration", NULL);
                report_error("[director.c] reg=fork()",1);
            }
        }
        else if(patients_in_queue<(N/3) && g_pids[1] > 0){
            LOG_PRINTF("|DIRECTOR| Queue is too short - closing second registration...");
            kill(g_pids[1], SIGUSR2);
            waitpid(g_pids[1], NULL, 0);
            g_pids[1] = 0;
        }

        //RANDOM SENDING OF SPECIALIZED DOCTORS TO WARD
        int random = rand() % 100;
        if(random==1){
            int sdoc = (rand() % 6)+3;

            if(g_pids[sdoc] > 0){
                LOG_PRINTF("|DIRECTOR| Sending specialized doctor %d to ward!", g_pids[sdoc]);
                kill(g_pids[sdoc], SIGUSR1);
            }
        }

        //sleep(1);
    }

    struct PatientStats *final_stats = shmat(g_shm_stats, NULL, 0);
    if(final_stats != (void *)-1) {
        LOG_PRINTF("\n========== FINAL REPORT ==========");
        LOG_PRINTF("Total patients generated: %d", final_stats->total_patients);
        LOG_PRINTF("Patients treated by PC Doctor: %d", final_stats->pc_doctor_count);
        LOG_PRINTF("Patients sent to Cardiologist: %d", final_stats->cardiologist_count);
        LOG_PRINTF("Patients sent to Neurologist: %d", final_stats->neurologist_count);
        LOG_PRINTF("Patients sent to Eye Doctor: %d", final_stats->eye_doctor_count);
        LOG_PRINTF("Patients sent to Laryngologist: %d", final_stats->laryngologist_count);
        LOG_PRINTF("Patients sent to Surgeon: %d", final_stats->surgeon_count);
        LOG_PRINTF("Patients sent to Pediatrician: %d", final_stats->pediatrician_count);
        LOG_PRINTF("Patients sent home: %d", final_stats->sent_home);
        LOG_PRINTF("==================================\n");
        shmdt(final_stats);
    }

    cleanup();
    
    printf("[DIRECTOR] ER evacuated!\n");

    return 0;
}

