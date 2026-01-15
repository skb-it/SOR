#include "common.h"
#include "errors.h"

void fill_pat_data(struct MsgBuffer *buf, int age, int is_guardian, int patient_id){

    //GETING PATIENT PID
    buf->patient_id = patient_id;

    //GETTING PATIENT AGE
    buf->age = age;
    
    //DRAWING - VIP(CHANCE OF 15%) OR COMMON(CHANCE OF 85%)
    if ((rand() % 100) < 15) buf->mtype = VIP;
    else                     buf->mtype = COMMON;

    buf->is_guardian= is_guardian;
}

void enter_waiting_room(int sem_id){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    int semop_enter_waiting_room = semop(sem_id, &sb, 1);
    if(semop_enter_waiting_room == -1) report_error("[patient.c] error: semop_enter_waiting_room", 1);
}

void leave_waiting_room(int sem_id){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    int semop_leave_waiting_room = semop(sem_id, &sb, 1);
    if(semop_leave_waiting_room == -1) report_error("[patient.c] error: semop_leave_waiting_room", 1);
}



int main(){

    int has_guardian = 0;
    //GENERATING PATIENT AGE AND CHECKING IF GUARDIAN IS REQUIRED
    srand(time(NULL) ^ getpid());
    int age = rand() % 117;

    int patient_id = getpid();
    

    if(age < 18){
        has_guardian = 1;
        pid_t pid = fork();

        if(pid == -1) report_error("[patient.c] error: fork()", 1);

        //GUARDIAN PROCESS
        if(pid > 0){
            printf("[GUARDIAN %d] Coming with my child.\n", getpid());
            wait(NULL);
            printf("[GUARDIAN %d] My child is examinated. Leaving ER with him...\n", getpid());

            return 0;
        }

        //GETTING PATIENT PID
        patient_id = getpid();
        
    }
                //GUARDIAN+CHILD PROCESS

    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] error: key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, 1, 0600);
    if(semget_waiting_room == -1) report_error("[patient.c] error: semget_waiting_room", 1);

    printf("[PATIENT %d] Trying to enter to the waiting room...\n", getpid());
    enter_waiting_room(semget_waiting_room);

    printf("[PATIENT %d] Entered to the waiting room!\n", getpid());
    //sleep(2);
    printf("[PATIENT %d] Registration number taken - waiting for my turn...\n", getpid());
    
    
    //FILLING PATIENT DATA
    struct MsgBuffer buf;
    fill_pat_data(&buf, age, has_guardian, patient_id);

    //MESSAGE QUEUE PATIENT<->REGISTRATION
    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[patient.c] key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600 | IPC_CREAT);
    if(msg_pat_reg == -1) report_error("[patient.c] msg_pat_reg", 1);


    //SHARING PATIENT DATA FOR REGISTRATION
    int msgsnd_pat_reg = msgsnd(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), 0);
    if(msgsnd_pat_reg == -1) report_error("[patient.c] error: msgsnd_pat_reg", 1);

    //LEAVING WAITING ROOM
    leave_waiting_room(semget_waiting_room);

    printf("[PATIENT %d] Registered! Waiting for PC doctor...\n", getpid());

    //MESSAGE QUEUE PATIENT<->PC DOCTOR
    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[patient.c] key_msg_doc_pat", 1);
    int msg_doc_pat = msgget(key_msg_doc_pat, 0600 | IPC_CREAT);
    if(msg_doc_pat == -1) report_error("[patient.c] msg_doc_pat", 1);

    struct PatientCard filled_card;

    int msgrcv_pat_doc = msgrcv(msg_doc_pat, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
    if (msgrcv_pat_doc == -1) report_error("[patient.c] msgrcv_doc_pat", 1);

    


    printf("|PATIENT %d| Examined. Going home...\n", getpid());

    return 0;
}