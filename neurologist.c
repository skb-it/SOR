#include "errors.h"
#include "common.h"

volatile int go_to_ward = 0;

void handle_signal(int sig) {
    go_to_ward = 1;
}

void visit_ward() {
    printf("|NEUROLOGIST %d| Received signal from DIRECTOR. Going to ward...\n", getpid());
    
    //int pause = (rand() % 5) + 3;
    //sleep(pause);
    
    printf("|NEUROLOGIST %d| Returned from ward to ER.\n", getpid());
    
    go_to_ward = 0;
}


int main(){
    signal(SIGUSR1, handle_signal);

    srand(time(NULL) ^ getpid());
    
    struct PatientCard filled_card;

    //MESSAGE QUEUE PATIENT<->NEUROLOGIST
    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[neurologist.c] error: key_msg_pat_neuro", 1);

    int msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(msg_pat_neuro == -1) report_error("[neurologist.c] error: msg_pat_neuro", 1);


    //SEMAPHORE MESSAGE QUEUE PATIENT<->NEUROLOGIST
    key_t key_sem_msg_pat_neuro = ftok(FTOK_PATH, ID_SEM_MSG_NEURO);
    if(key_sem_msg_pat_neuro == -1) report_error("[director.c] key_sem_msg_pat_neuro", 1);

    int semget_msg_pat_neuro = semget(key_sem_msg_pat_neuro, 1, 0600 | IPC_CREAT);
    if(semget_msg_pat_neuro == -1) report_error("[director.c] semget_msg_pat_neuro", 1);

    while(1){
        if(go_to_ward) {
            visit_ward();
        }

        printf("|NEUROLOGIST %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_neuro = msgrcv(msg_pat_neuro, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if (msgrcv_pat_neuro == -1) {
            if (errno == EINTR) {
                if(go_to_ward) {
                    visit_ward();
                }
                continue;
            } 
            else {
                report_error("[neurologist.c] msgrcv_pat_neurologist", 1);
            }
        }

        printf("|NEUROLOGIST %d| Patient %d came! Starting examination...\n", getpid(), filled_card.patient_id);

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

        filled_card.mtype = filled_card.patient_id;

        int msg_sent = 0;
        while (msg_sent != 1) {
            int msgsnd_pat_neuro = msgsnd(msg_pat_neuro, &filled_card, sizeof(filled_card) - sizeof(long), 0);
            if (msgsnd_pat_neuro == -1) {
                if (errno == EINTR) {
                    if(go_to_ward) {
                        visit_ward();
                    }
                    continue; 
                } 
                else {
                    report_error("[neurologist.c] msgsnd_pat_neuro", 1);
                }
            }
            msg_sent = 1;
        }

        printf("|NEUROLOGIST %d| Patient %d examinated!\n", getpid(), filled_card.patient_id);

        free_slot(semget_msg_pat_neuro);

        if(go_to_ward) {
            visit_ward();
        }
    }


    return 0;
}