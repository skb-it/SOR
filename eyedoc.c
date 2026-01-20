#include "errors.h"
#include "common.h"

volatile int go_to_ward = 0;

void handle_signal(int sig) {
    go_to_ward = 1;
}

void visit_ward() {
    printf("|EYE DOCTOR %d| Received signal from DIRECTOR. Going to ward...\n", getpid());
    
    //int pause = (rand() % 5) + 3;
    //sleep(pause);
    
    printf("|EYE DOCTOR %d| Returned from ward to ER.\n", getpid());
    
    go_to_ward = 0;
}



int main(){
    signal(SIGUSR1, handle_signal);

    srand(time(NULL));
    
    struct PatientCard filled_card;

    key_t key_msg_pat_eyedoc = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
    if(key_msg_pat_eyedoc == -1) report_error("[eyedoc.c] error: key_msg_pat_eyedoc", 1);

    int msg_pat_eyedoc = msgget(key_msg_pat_eyedoc, 0600 | IPC_CREAT);
    if(msg_pat_eyedoc == -1) report_error("[eyedoc.c] error: msg_pat_eyedoc", 1);

    while(1){
        if(go_to_ward) {
            visit_ward();
        }

        printf("|EYE DOCTOR %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_eyedoc = msgrcv(msg_pat_eyedoc, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);

        if (msgrcv_pat_eyedoc == -1) {
            if (errno == EINTR) {
                if(go_to_ward) {
                    visit_ward();
                }
                continue;
            } 
            else {
                report_error("[eyedoc.c] msgrcv_pat_cardio", 1);
            }
        }

        printf("|EYE DOCTOR %d| Patient %d came! Starting examination...\n", getpid(), filled_card.patient_id);

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


        int msgsnd_pat_eyedoc = msgsnd(msg_pat_eyedoc, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_eyedoc == -1) report_error("[eyedoc.c] error: msgsnd_pat_eyedoc", 1);

        printf("|EYE DOCTOR %d| Patient %d examinated!\n", getpid(), filled_card.patient_id);

        if(go_to_ward) {
            visit_ward();
        }
    }


    return 0;
}