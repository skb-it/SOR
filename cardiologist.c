#include "errors.h"
#include "common.h"

int main(){
    srand(time(NULL));
    
    struct PatientCard filled_card;

    key_t key_msg_pat_cardio = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
    if(key_msg_pat_cardio == -1) report_error("[cardiologist.c] error: key_msg_pat_cardio", 1);

    int msg_pat_cardio = msgget(key_msg_pat_cardio, 0600 | IPC_CREAT);
    if(msg_pat_cardio == -1) report_error("[cardiologist.c] error: msg_pat_cardio", 1);

    while(1){
        printf("|CARDIOLOGIST %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_doc = msgrcv(msg_pat_cardio, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);
        if (msgrcv_pat_doc == -1) report_error("[cardiologist.c] msgrcv_doc_pat", 1);

        printf("|CARDIOLOGIST %d| Patient %d came! Starting examination...\n", getpid());

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

        int msgsnd_pat_cardio = msgsnd(msg_pat_cardio, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_cardio == -1) report_error("[catdiologist.c] error: msgsnd_pat_cardio", 1);

        printf("|CARDIOLOGIST %d| Patient %d examinated!\n", getpid());
    }


    return 0;
}