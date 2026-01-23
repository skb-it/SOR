#include "errors.h"
#include "common.h"

volatile int go_to_ward = 0;

void handle_signal(int sig) {
    go_to_ward = 1;
}

void visit_ward() {
    LOG_PRINTF("|EYE DOCTOR %d| Received signal from DIRECTOR. Going to ward...", getpid());
    
    //int pause = (rand() % 5) + 3;
    //sleep(pause);
    
    LOG_PRINTF("|EYE DOCTOR %d| Returned from ward to ER.", getpid());
    
    go_to_ward = 0;
}


int main(){
    signal(SIGUSR1, handle_signal);

    srand(time(NULL) ^ getpid());
    
    struct PatientCard filled_card;

    //MESSAGE QUEUE PATIENT<->EYE DOCTOR
    key_t key_msg_pat_eyedoc = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eyedoc == -1) report_error("[eyedoc.c] error: key_msg_pat_eyedoc", 1);

    int msg_pat_eyedoc = msgget(key_msg_pat_eyedoc, 0600 | IPC_CREAT);
    if(msg_pat_eyedoc == -1) report_error("[eyedoc.c] error: msg_pat_eyedoc", 1);

    //SEMAPHORE MESSAGE QUEUE PATIENT<->EYE DOCTOR
    key_t key_sem_msg_pat_eyedoc = ftok(FTOK_PATH, ID_SEM_MSG_EYEDOC);
    if(key_sem_msg_pat_eyedoc == -1) report_error("[director.c] key_sem_msg_pat_eyedoc", 1);

    int semget_msg_pat_eyedoc = semget(key_sem_msg_pat_eyedoc, 1, 0600 | IPC_CREAT);
    if(semget_msg_pat_eyedoc == -1) report_error("[director.c] semget_msg_pat_eyedoc", 1);


    while(1){
        if(go_to_ward) {
            visit_ward();
        }

        LOG_PRINTF("|EYE DOCTOR %d| Waiting for a patient...", getpid());

        int msgrcv_pat_eyedoc = msgrcv(msg_pat_eyedoc, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if (msgrcv_pat_eyedoc == -1) {
            if (errno == EINTR) {
                if(go_to_ward) {
                    visit_ward();
                }
                continue;
            } 
            else {
                report_error("[eyedoc.c] msgrcv_pat_eyedoc", 1);
            }
        }

        LOG_PRINTF("|EYE DOCTOR %d| Patient %d came! Starting examination...", getpid(), filled_card.patient_id);

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
            int msgsnd_pat_eyedoc = msgsnd(msg_pat_eyedoc, &filled_card, sizeof(filled_card) - sizeof(long), 0);
            if (msgsnd_pat_eyedoc == -1) {
                if (errno == EINTR) {
                    if(go_to_ward) {
                        visit_ward();
                    }
                    continue; 
                } 
                else {
                    report_error("[eyedoc.c] msgsnd_pat_eyedoc", 1);
                }
            }
            msg_sent = 1;
        }

        LOG_PRINTF("|EYE DOCTOR %d| Patient %d examinated!", getpid(), filled_card.patient_id);

        free_slot(semget_msg_pat_eyedoc);
        increment_doctor_count(DOC_EYE_DOC);
        if(go_to_ward) {
            visit_ward();
        }
    }


    return 0;
}