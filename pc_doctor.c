#include "common.h"
#include "errors.h"

volatile sig_atomic_t go_to_ward_requested = 0;

void handle_signal(int sig) {
    go_to_ward_requested = 1;
}

void go_to_ward_break() {
    printf("\n<<<|DOCTOR %d| RECEIVED SIGNAL! Going to ward...\n", getpid());
    int break_time = (rand() % 5) + 3;
    //sleep(break_time);
    printf(">>>|DOCTOR %d| Returned from ward to ER.\n\n", getpid());
    go_to_ward_requested = 0;
}

void asses_doc(struct PatientCard *card){
    int random = rand() % 100;

    if(card->age<18){
        if(random< 20){
            card->doc=DOC_PEDIATRICIAN;
        }
        else if(random<40){
            card->doc=DOC_EYE_DOC;
        }
        else if(random<60){
            card->doc=DOC_LARYNGOLOGIST;
        }
        else if(random<80){
            card->doc=DOC_NEUROLOGIST;
        }
        else if(random<80){
            card->doc=DOC_CARDIOLOGIST;
        }
        else{
            card->doc=DOC_SURGEON;
        }
    }
    else {
        if(random< 20){
            card->doc=DOC_CARDIOLOGIST;
        }
        else if(random<40){
            card->doc=DOC_EYE_DOC;
        }
        else if(random<60){
            card->doc=DOC_LARYNGOLOGIST;
        }
        else if(random<80){
            card->doc=DOC_NEUROLOGIST;
        }
        else{
            card->doc=DOC_SURGEON;
        }
    }
}

void triage(struct PatientCard *card){
    printf("|DOCTOR %d| Examining Patient %d...)", getpid(), card->patient_id);
    //sleep(2);

    int random = rand() % 100;
    
    if (random < 10) {
        card->triage = TRIAGE_RED;
        printf("|DOCTOR %d| PATIENT %d -> RED TRIAGE\n", getpid(), card->patient_id);
    } else if (random < 40) {
        card->triage = TRIAGE_YELLOW;
        printf("|DOCTOR %d| PATIENT %d -> YELLOW TRIAGE\n", getpid(), card->patient_id);
    } else if (random < 80) {
        card->triage = TRIAGE_GREEN;
        printf("|DOCTOR %d| PATIENT %d -> GREEN) TRIAGE\n", getpid(), card->patient_id);
    } else {
        card->triage = SENT_HOME;
        printf("|DOCTOR %d| PATIENT %d -> SENT TO HOME\n", getpid(), card->patient_id);
    }
}



int main(){
    srand(time(NULL) ^ getpid());

    signal(SIGUSR1, handle_signal);

    //SHARED MEMORY REGISTRATION->PC DOCTOR
    key_t key_shm_reg_doc = ftok(FTOK_PATH, ID_SHM_REG_DOC);
    if(key_shm_reg_doc == -1) report_error("[pc_doctor.c] error: key_shm_reg_doc", 1);

    int shmget_reg_doc = shmget(key_shm_reg_doc, SHM_SIZE_CARD, 0600 | IPC_CREAT);
    if(shmget_reg_doc == -1) report_error("[pc_doctor.c] error: shmget_reg_doc", 1);

    struct PatientCard *card = shmat(shmget_reg_doc, NULL, 0);
    if(card == (void *)-1) report_error("[pc_doctor.c] error: shmat (reg->doc)", 1);

    //SEMAPHORE DOCTOR
    key_t key_sem_doc = ftok(FTOK_PATH, ID_SEM_DOC);
    if(key_sem_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    int semget_doc = semget(key_sem_doc, 2, 0600);
    if(semget_doc == -1) report_error("[pc_doctor.c] error: key_sem_doc", 1);

    struct sembuf wait_for_data;
    wait_for_data.sem_num = 1;
    wait_for_data.sem_op = -1;
    wait_for_data.sem_flg = 0;

    struct sembuf signal_slot_free;
    signal_slot_free.sem_num = 0;
    signal_slot_free.sem_op = 1;
    signal_slot_free.sem_flg = 0;


    while(1){
        if (go_to_ward_requested) {
            go_to_ward_break();
        }

        printf("|DOCTOR %d| Waiting for data...\n", getpid());

        int semop_wait = semop(semget_doc, &wait_for_data, 1);
        if(semop_wait == -1){
            if (errno == EINTR) {
                if (go_to_ward_requested) go_to_ward_break();
                continue; 
            } else {
                report_error("semop wait failed", 1);
            }
        }

            struct PatientCard local_card;
            local_card.patient_id = card->patient_id;
            local_card.triage = card->triage;
            local_card.age = card->age;
            local_card.flag = card->flag;
            local_card.is_guardian = card->is_guardian;
            local_card.is_vip = card->is_vip;
            local_card.mtype = card->mtype;

            printf("|DOCTOR %d| Received Patient %d data!\n", getpid(), local_card.patient_id);

            int semop_slot_free = semop(semget_doc, &signal_slot_free, 1);
            if (semop_slot_free == -1) report_error("[pc_doctor.c] error: semop_slot_free", 1);


            triage(&local_card);

            //sleep(1);
            asses_doc(&local_card);

            if (go_to_ward_requested) {
                go_to_ward_break();
        }
    }


    
    int shmdt_reg_doc = shmdt(card);
    if(shmdt_reg_doc == -1) report_error("[pc_doctor.c] error: shmdt_reg_doc", 1);

    return 0;
}