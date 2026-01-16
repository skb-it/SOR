#include "errors.h"
#include "common.h"

int main(){
    srand(time(NULL));
    
    struct PatientCard filled_card;

    key_t key_msg_pat_pedatr = ftok(FTOK_PATH, ID_MSG_PAT_PEDATR);
    if(key_msg_pat_pedatr == -1) report_error("[pedatrician.c] error: key_msg_pat_pedatr", 1);

    int msg_pat_pedatr = msgget(key_msg_pat_pedatr, 0600 | IPC_CREAT);
    if(msg_pat_pedatr == -1) report_error("[pedatrician.c] error: msg_pat_pedatr", 1);

    while(1){
        printf("|PEDATRICIAN %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_pedatr = msgrcv(msg_pat_pedatr, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);
        if (msgrcv_pat_pedatr == -1) report_error("[pedatrician.c] msgrcv_pat_pedatr", 1);

        printf("|PEDATRICIAN %d| Patient %d came! Starting examination...\n", getpid(), filled_card.patient_id);

        int random = rand() % 1000;

        if(random <= 145){
            filled_card.sdoc_dec = SENT_TO_WARD;
        }  
        else if(random <=050){
            filled_card.sdoc_dec = OTHER_S_HOSP;
        }
        else{
            filled_card.sdoc_dec = SENT_HOME;
        }

        int msgsnd_pat_pedatr = msgsnd(msg_pat_pedatr, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_pedatr == -1) report_error("[pedatrician.c] error: msgsnd_pat_pedatr", 1);

        printf("|PEDATRICIAN %d| Patient %d examinated!\n", getpid(), filled_card.patient_id);
    }


    return 0;
}