#include "errors.h"
#include "common.h"

volatile int go_to_ward = 0;

void handle_signal(int sig) {
    go_to_ward = 1;
}

void visit_ward() {
    printf("|SURGEON %d| Received signal from DIRECTOR. Going to ward...\n", getpid());
    
    //int pause = (rand() % 5) + 3;
    //sleep(pause);
    
    printf("|SURGEON %d| Returned from ward to ER.\n", getpid());
    
    go_to_ward = 0;
}

void free_slot(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    int semop_free_slot_cardio = semop(semget, &sb, 1);
    if(semop_free_slot_cardio == -1) report_error("[patient.c] semop_free_slot_cardio", 1);
}

int main(){
    signal(SIGUSR1, handle_signal);

    srand(time(NULL));
    
    struct PatientCard filled_card;


    //MESSAGE QUEUE PATIENT<->SURGEON
    key_t key_msg_pat_surgeon = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
    if(key_msg_pat_surgeon == -1) report_error("[surgeon.c] error: key_msg_pat_surgeon", 1);

    int msg_pat_surgeon = msgget(key_msg_pat_surgeon, 0600 | IPC_CREAT);
    if(msg_pat_surgeon == -1) report_error("[surgeon.c] error: msg_pat_surgeon", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->SURGEON
    key_t key_sem_msg_pat_surgeon = ftok(FTOK_PATH, ID_SEM_MSG_SURGEON);
    if(key_sem_msg_pat_surgeon == -1) report_error("[director.c] key_sem_msg_pat_surgeon", 1);

    int semget_msg_pat_surgeon = semget(key_sem_msg_pat_surgeon, 1, 0600 | IPC_CREAT);
    if(semget_msg_pat_surgeon == -1) report_error("[director.c] semget_msg_pat_surgeon", 1);


    while(1){
        if(go_to_ward) {
            visit_ward();
        }

        printf("|SURGEON %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_surgeon = msgrcv(msg_pat_surgeon, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if (msgrcv_pat_surgeon == -1) {
            if (errno == EINTR) {
                if(go_to_ward) {
                    visit_ward();
                }
                continue;
            }
            else {
                report_error("[surgeon.c] msgrcv_pat_surgeon", 1);
            }
        }

        printf("|SURGEON %d| Patient %d came! Starting examination...\n", getpid(), filled_card.patient_id);

        int random = rand() % 1000;

        if(random < 145){
            filled_card.sdoc_dec = SENT_TO_WARD;
        }
        else if(random < 150){
            filled_card.sdoc_dec = OTHER_S_HOSP;
        }
        else{
            filled_card.sdoc_dec = SENT_HOME;
        }

        int msg_sent = 0;
        while (msg_sent != 1) {
            int msgsnd_pat_surgeon = msgsnd(msg_pat_surgeon, &filled_card, sizeof(filled_card) - sizeof(long), 0);
            if (msgsnd_pat_surgeon == -1) {
                if (errno == EINTR) {
                    if(go_to_ward) {
                        visit_ward();
                    }
                    continue; 
                } 
                else {
                    report_error("[surgeon.c] msgsnd_pat_surgeon", 1);
                }
            }
            msg_sent = 1;
        }

        printf("|SURGEON %d| Patient %d examinated!\n", getpid(), filled_card.patient_id);

        free_slot(semget_msg_pat_surgeon);

        if(go_to_ward) {
            visit_ward();
        }
    }


    return 0;
}