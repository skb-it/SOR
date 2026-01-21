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

void enter_waiting_room(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    int semop_enter_waiting_room = semop(semget, &sb, 1);
    if(semop_enter_waiting_room == -1) report_error("[patient.c] semop_enter_waiting_room", 1);
}

void leave_waiting_room(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    int semop_leave_waiting_room = semop(semget, &sb, 1);
    if(semop_leave_waiting_room == -1) report_error("[patient.c] semop_leave_waiting_room", 1);
}

void reserve_queue_place(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    int semop_give_info = semop(semget, &sb, 1);
    if(semop_give_info == -1) report_error("[patient.c] semop_give_info", 1);
}

int main(){

    int has_guardian = 0;
    //GENERATING PATIENT AGE AND CHECKING IF GUARDIAN IS REQUIRED
    srand(time(NULL)^getpid());
    int age = rand() % 117;

    int patient_id = getpid();
    

    if(age < 18){
        has_guardian = 1;
        pid_t pid = fork();
        if(pid == -1) {
            report_error("[patient.c] pid = fork()", 1);
        }

        //GUARDIAN PROCESS
        if(pid > 0){
            printf("[GUARDIAN %d] Coming with my child.\n", getpid());
            wait(NULL);
            printf("[GUARDIAN %d] My child is examinated.\n", getpid());

            return 0;
        }

        //GETTING PATIENT PID
        patient_id = getpid();
        
    }
                //GUARDIAN+CHILD PROCESS

    //SEMAPHORE WAITING ROOM
    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] key_sem_waiting_room", 1);

    int semget_waiting_room = semget(key_sem_waiting_room, 1, 0600);
    if(semget_waiting_room == -1) report_error("[patient.c] semget_waiting_room", 1);

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

    //SEMAPHORE MESSAGE QUEUE PATIENT->REGISTRATION
    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[registration.c] key_sem_msg_pat_reg", 1);

    int semget_msg_pat_reg = semget(key_sem_msg_pat_reg, 0, 0600);
    if(semget_msg_pat_reg == -1) report_error("[registration.c] semget_msg_pat_reg", 1);

    reserve_queue_place(semget_msg_pat_reg);

    //SHARING PATIENT DATA FOR REGISTRATION
    int msgsnd_pat_reg = msgsnd(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), 0);
    if(msgsnd_pat_reg == -1) report_error("[patient.c] msgsnd_pat_reg", 1);


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



    //MESSAGE QUEUE PATIENT->CARDIOLOGIST
    key_t key_msg_pat_cardio = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio == -1) report_error("[patient.c] key_msg_pat_cardio", 1);

    int msg_pat_cardio = msgget(key_msg_pat_cardio, 0600 | IPC_CREAT);
    if(msg_pat_cardio == -1) report_error("[patient.c] msg_pat_cardio", 1);


    //MESSAGE QUEUE PATIENT->NEUROLOGIST
    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[patient.c] key_msg_pat_neuro", 1);

    int msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(msg_pat_neuro == -1) report_error("[patient.c] msg_pat_neuro", 1);


    //MESSAGE QUEUE PATIENT->EYE DOC
    key_t key_msg_pat_eye = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eye == -1) report_error("[patient.c] key_msg_pat_eye", 1);

    int msg_pat_eye = msgget(key_msg_pat_eye, 0600 | IPC_CREAT);
    if(msg_pat_eye == -1) report_error("[patient.c] msg_pat_eye", 1);


    //MESSAGE QUEUE PATIENT->LARYNGOLOGIST
    key_t key_msg_pat_laryng = ftok(FTOK_PATH, ID_MSG_PAT_LARYNG);
    if(key_msg_pat_laryng == -1) report_error("[patient.c] key_msg_pat_laryng", 1);

    int msg_pat_laryng = msgget(key_msg_pat_laryng, 0600 | IPC_CREAT);
    if(msg_pat_laryng == -1) report_error("[patient.c] msg_pat_laryng", 1);

    //MESSAGE QUEUE PATIENT->SURGEON
    key_t key_msg_pat_surgeon = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon == -1) report_error("[patient.c] key_msg_pat_surgeon", 1);

    int msg_pat_surgeon = msgget(key_msg_pat_surgeon, 0600 | IPC_CREAT);
    if(msg_pat_surgeon == -1) report_error("[patient.c] msg_pat_surgeon", 1);

    //MESSAGE QUEUE PATIENT->PEDATRICIAN
    key_t key_msg_pat_pediatr = ftok(FTOK_PATH, ID_MSG_PAT_PEDIATR);
    if(key_msg_pat_pediatr == -1) report_error("[patient.c] key_msg_pat_pediatr", 1);

    int msg_pat_pediatr = msgget(key_msg_pat_pediatr, 0600 | IPC_CREAT);
    if(msg_pat_pediatr == -1) report_error("[patient.c] msg_pat_pediatr", 1);

    if(filled_card.is_vip == 1){
        filled_card.mtype = VIP;
    }
    else{
        filled_card.mtype = COMMON;
    }

    if(filled_card.sdoc == DOC_CARDIOLOGIST){
        key_t key_sem_msg_pat_cardio = ftok(FTOK_PATH, ID_SEM_MSG_CARDIO);
        if(key_sem_msg_pat_cardio == -1) report_error("[director.c] key_sem_msg_pat_cardio", 1);

        int semget_msg_pat_cardio = semget(key_sem_msg_pat_cardio, 1, 0600 | IPC_CREAT); //  16384:32=512, sizeof(struct PatientCard) = 32
        if(semget_msg_pat_cardio == -1) report_error("[director.c] semget_msg_pat_cardio", 1);

        reserve_queue_place(semget_msg_pat_cardio);

        int msgsnd_pat_cardio = msgsnd(msg_pat_cardio, &filled_card, sizeof(struct PatientCard) - sizeof(long), 0);
        if(msgsnd_pat_cardio == -1) report_error("[patient.c] msgsnd_pat_cardio", 1);

        int msgrcv_pat_cardio = msgrcv(msg_pat_cardio, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_cardio == -1) report_error("[patient.c] msgrcv_pat_cardio", 1);

        return 0;
    }
    else if (filled_card.sdoc == DOC_EYE_DOC){
        key_t key_sem_msg_pat_eyedoc = ftok(FTOK_PATH, ID_SEM_MSG_EYEDOC);
        if(key_sem_msg_pat_eyedoc == -1) report_error("[director.c] key_sem_msg_pat_eyedoc", 1);

        int semget_msg_pat_eyedoc = semget(key_sem_msg_pat_eyedoc, 1, 0600 | IPC_CREAT); //  16384:32=1024, sizeof(struct PatientCard) = 32
        if(semget_msg_pat_eyedoc == -1) report_error("[director.c] semget_msg_pat_eyedoc", 1);

        reserve_queue_place(semget_msg_pat_eyedoc);
        
        int msgsnd_pat_eye = msgsnd(msg_pat_eye, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_eye == -1) report_error("[patient.c] msgsnd_pat_eye", 1);

        int msgrcv_pat_eye = msgrcv(msg_pat_eye, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_eye == -1) report_error("[patient.c] msgrcv_pat_eye", 1);

        return 0;
    }
    else if (filled_card.sdoc == DOC_LARYNGOLOGIST){
        key_t key_sem_msg_pat_laryng = ftok(FTOK_PATH, ID_SEM_MSG_LARYNG);
        if(key_sem_msg_pat_laryng == -1) report_error("[director.c] key_sem_msg_pat_laryng", 1);

        int semget_msg_pat_laryng = semget(key_sem_msg_pat_laryng, 1, 0600 | IPC_CREAT); //  16384:32=1024, sizeof(struct PatientCard) = 32
        if(semget_msg_pat_laryng == -1) report_error("[director.c] semget_msg_pat_laryng", 1);

        reserve_queue_place(semget_msg_pat_laryng);

        int msgsnd_pat_laryng = msgsnd(msg_pat_laryng, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_laryng == -1) report_error("[patient.c] msgsnd_pat_laryng", 1);

        int msgrcv_pat_laryng = msgrcv(msg_pat_laryng, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_laryng == -1) report_error("[patient.c] msgrcv_pat_laryng", 1);

        return 0;
    }
    else if (filled_card.sdoc == DOC_NEUROLOGIST){
        key_t key_sem_msg_pat_neuro = ftok(FTOK_PATH, ID_SEM_MSG_NEURO);
        if(key_sem_msg_pat_neuro == -1) report_error("[director.c] key_sem_msg_pat_neuro", 1);

        int semget_msg_pat_neuro = semget(key_sem_msg_pat_neuro, 1, 0600 | IPC_CREAT); //  16384:32=1024, sizeof(struct PatientCard) = 32
        if(semget_msg_pat_neuro == -1) report_error("[director.c] semget_msg_pat_neuro", 1);

        reserve_queue_place(semget_msg_pat_neuro);

        int msgsnd_pat_neuro = msgsnd(msg_pat_neuro, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_neuro == -1) report_error("[patient.c] msgsnd_pat_neuro", 1);

        int msgrcv_pat_neuro = msgrcv(msg_pat_neuro, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_neuro == -1) report_error("[patient.c] msgrcv_pat_neuro", 1);

        return 0;
    }
    else if (filled_card.sdoc == DOC_PEDIATRICIAN){
        key_t key_sem_msg_pat_pediatr = ftok(FTOK_PATH, ID_SEM_MSG_PEDIATR);
        if(key_sem_msg_pat_pediatr == -1) report_error("[director.c] key_sem_msg_pat_pediatr", 1);

        int semget_msg_pat_pediatr = semget(key_sem_msg_pat_pediatr, 1, 0600 | IPC_CREAT);
        if(semget_msg_pat_pediatr == -1) report_error("[director.c] semget_msg_pat_pediatr", 1);

        reserve_queue_place(semget_msg_pat_pediatr);

        int msgsnd_pat_pediatr = msgsnd(msg_pat_pediatr, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_pediatr == -1) report_error("[patient.c] msgsnd_pat_pediatr", 1);

        int msgrcv_pat_pediatr = msgrcv(msg_pat_pediatr, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_pediatr == -1) report_error("[patient.c] msgrcv_pat_pediatr", 1);

        return 0;
    }
    else if (filled_card.sdoc == DOC_SURGEON){
        key_t key_sem_msg_pat_surgeon = ftok(FTOK_PATH, ID_SEM_MSG_SURGEON);
        if(key_sem_msg_pat_surgeon == -1) report_error("[director.c] key_sem_msg_pat_surgeon", 1);

        int semget_msg_pat_surgeon = semget(key_sem_msg_pat_surgeon, 1, 0600 | IPC_CREAT);
        if(semget_msg_pat_surgeon == -1) report_error("[director.c] semget_msg_pat_surgeon", 1);

        reserve_queue_place(semget_msg_pat_surgeon);

        int msgsnd_pat_surgeon = msgsnd(msg_pat_surgeon, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_surgeon == -1) report_error("[patient.c] msgsnd_pat_surgeon", 1);

        int msgrcv_pat_surgeon = msgrcv(msg_pat_surgeon, &filled_card, sizeof(struct PatientCard) - sizeof(long), getpid(), 0);
        if (msgrcv_pat_surgeon == -1) report_error("[patient.c] msgrcv_pat_surgeon", 1);

        return 0;
    }
    else{
        printf("|PATIENT %d| Examined by specialist doctor!\n", getpid());

        return 0;
    }

}