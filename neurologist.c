#include "errors.h"
#include "common.h"

int main(){
    srand(time(NULL));
    
    struct PatientCard filled_card;

    key_t key_msg_pat_neuro = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
    if(key_msg_pat_neuro == -1) report_error("[neurologist.c] error: key_msg_pat_neuro", 1);

    int msg_pat_neuro = msgget(key_msg_pat_neuro, 0600 | IPC_CREAT);
    if(msg_pat_neuro == -1) report_error("[neurologist.c] error: msg_pat_neuro", 1);

    while(1){
        printf("|NEUROLOGIST %d| Waiting for a patient...\n", getpid());

        int msgrcv_pat_neuro = msgrcv(msg_pat_neuro, &filled_card, sizeof(struct PatientCard) - sizeof(long), -3, 0);
        if (msgrcv_pat_neuro == -1) report_error("[neurologist.c] msgrcv_pat_neuro", 1);

        printf("|NEUROLOGIST %d| Patient %d came! Starting examination...\n", getpid(), filled_card.patient_id);

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

        int msgsnd_pat_neuro = msgsnd(msg_pat_neuro, &filled_card, sizeof(filled_card) - sizeof(long), 0);
        if(msgsnd_pat_neuro == -1) report_error("[neurologist.c] error: msgsnd_pat_neuro", 1);

        printf("|NEUROLOGIST %d| Patient %d examinated!\n", getpid(), filled_card.patient_id);
    }


    return 0;
}