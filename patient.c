#include "common.h"
#include "errors.h"

void fill_pat_data(struct Msg *buf, int age, int is_guardian, pid_t patient_id){
    buf->patient_id = patient_id;
    buf->age = age;
    buf->is_guardian = is_guardian;
    
    if ((rand() % 100) < 15) 
        buf->mtype = VIP;
    else                     
        buf->mtype = COMMON;
}

int main(){
    srand(time(NULL) ^ getpid());
    
    int has_guardian = 0;
    int age = rand() % 117;
    pid_t patient_id = getpid();

    if(age < 18){
        has_guardian = 1;
        LOG_PRINTF("|PATIENT %d| Child (age %d) arrived with guardian.", patient_id, age);
    }

    key_t key_sem_waiting_room = ftok(FTOK_PATH, ID_SEM_WAITING_ROOM);
    if(key_sem_waiting_room == -1) report_error("[patient.c] key_sem_waiting_room", 1);

    int sem_waiting_room = semget(key_sem_waiting_room, 1, 0600);
    if(sem_waiting_room == -1) report_error("[patient.c] sem_waiting_room", 1);

    LOG_PRINTF("|PATIENT %d| Trying to enter waiting room%s...", patient_id, 
               has_guardian ? " (with guardian)" : "");
    
    if(has_guardian) {
        if(sem_acquire(sem_waiting_room) == -1) {
            report_error("[patient.c] enter waiting room (1)", 1);
        }
        if(sem_acquire(sem_waiting_room) == -1) {
            sem_release(sem_waiting_room);
            report_error("[patient.c] enter waiting room (2)", 1);
        }
    } else {
        if(sem_acquire(sem_waiting_room) == -1) {
            report_error("[patient.c] enter waiting room", 1);
        }
    }

    LOG_PRINTF("|PATIENT %d| Entered waiting room%s, taking registration number.", 
               patient_id, has_guardian ? " (with guardian)" : "");

    key_t key_msg_pat_reg = ftok(FTOK_PATH, ID_MSG_PAT_REG);
    if(key_msg_pat_reg == -1) report_error("[patient.c] key_msg_pat_reg", 1);

    int msg_pat_reg = msgget(key_msg_pat_reg, 0600);
    if(msg_pat_reg == -1) report_error("[patient.c] msg_pat_reg", 1);

    key_t key_sem_msg_pat_reg = ftok(FTOK_PATH, ID_SEM_MSG_REG);
    if(key_sem_msg_pat_reg == -1) report_error("[patient.c] key_sem_msg_pat_reg", 1);

    int sem_msg_pat_reg = semget(key_sem_msg_pat_reg, 1, 0600);
    if(sem_msg_pat_reg == -1) report_error("[patient.c] sem_msg_pat_reg", 1);

    //RESERVE QUEUE SLOT TO REGISTRATION
    if(sem_acquire(sem_msg_pat_reg) == -1) {
        report_error("[patient.c] acquire registration queue", 1);
    }

    struct Msg buf;
    fill_pat_data(&buf, age, has_guardian, patient_id);

    if(msgsnd(msg_pat_reg, &buf, sizeof(buf) - sizeof(long), 0) == -1) {
        report_error("[patient.c] msgsnd to registration", 1);
    }
    
    //LEAVING WAITING ROOM
    sem_release(sem_waiting_room);
    if(has_guardian) {
        sem_release(sem_waiting_room);
    }

    LOG_PRINTF("|PATIENT %d| Registered%s, waiting for PC doctor...", 
               patient_id, has_guardian ? " (with guardian)" : "");

    key_t key_msg_doc_pat = ftok(FTOK_PATH, ID_MSG_PAT_DOC);
    if(key_msg_doc_pat == -1) report_error("[patient.c] key_msg_doc_pat", 1);

    int msg_doc_pat = msgget(key_msg_doc_pat, 0600);
    if(msg_doc_pat == -1) report_error("[patient.c] msg_doc_pat", 1);

    struct PatientCard filled_card;
    while(msgrcv(msg_doc_pat, &filled_card, sizeof(struct PatientCard) - sizeof(long), patient_id, 0) == -1) {
        if(errno == EINTR) continue;
        report_error("[patient.c] msgrcv_doc_pat", 1);
    }

    LOG_PRINTF("|PATIENT %d| Received diagnosis from PC doctor, triage=%d%s", 
               patient_id, filled_card.triage, has_guardian ? " (with guardian)" : "");

    if(filled_card.triage == SENT_HOME){
        LOG_PRINTF("|PATIENT %d| Sent home by PC doctor%s, leaving.", 
                   patient_id, has_guardian ? " (with guardian)" : "");

        key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
        if(key_sem_gen != -1) {
            int sem_gen = semget(key_sem_gen, 1, 0600);
            if(sem_gen != -1) {
                sem_release(sem_gen);
            }
        }

        LOG_PRINTF("|PATIENT %d| Leaving hospital%s.", 
                   patient_id, has_guardian ? " (with guardian)" : "");
        return 0;
    }

    int msg_specialist = -1;
    int sem_specialist = -1;
    const char *specialist_name = "";
    
    key_t key_msg, key_sem;
    char sem_id_char;
    
    switch(filled_card.sdoc) {
        case DOC_CARDIOLOGIST:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_CARDIO);
            sem_id_char = ID_SEM_MSG_CARDIO;
            specialist_name = "CARDIOLOGIST";
            break;
        case DOC_NEUROLOGIST:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_NEURO);
            sem_id_char = ID_SEM_MSG_NEURO;
            specialist_name = "NEUROLOGIST";
            break;
        case DOC_EYE_DOC:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_EYE);
            sem_id_char = ID_SEM_MSG_EYEDOC;
            specialist_name = "EYE DOCTOR";
            break;
        case DOC_LARYNGOLOGIST:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_LARYNG);
            sem_id_char = ID_SEM_MSG_LARYNG;
            specialist_name = "LARYNGOLOGIST";
            break;
        case DOC_SURGEON:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_SURGEON);
            sem_id_char = ID_SEM_MSG_SURGEON;
            specialist_name = "SURGEON";
            break;
        case DOC_PEDIATRICIAN:
            key_msg = ftok(FTOK_PATH, ID_MSG_PAT_PEDIATR);
            sem_id_char = ID_SEM_MSG_PEDIATR;
            specialist_name = "PEDIATRICIAN";
            break;
        default:
            LOG_PRINTF("|PATIENT %d| Unknown specialist assigned, leaving.", patient_id);
            key_t key_sem_gen = ftok(FTOK_PATH, ID_SEM_GEN);
            if(key_sem_gen != -1) {
                int sem_gen = semget(key_sem_gen, 1, 0600);
                if(sem_gen != -1) {
                    sem_release(sem_gen);
                }
            }
            LOG_PRINTF("|PATIENT %d| Leaving hospital.", patient_id);
            return 0;
    }
    
    if(key_msg == -1) report_error("[patient.c] ftok specialist msg", 1);
    
    msg_specialist = msgget(key_msg, 0600);
    if(msg_specialist == -1) report_error("[patient.c] msgget specialist", 1);
    
    key_sem = ftok(FTOK_PATH, sem_id_char);
    if(key_sem == -1) report_error("[patient.c] ftok specialist sem", 1);
    
    sem_specialist = semget(key_sem, 1, 0600);
    if(sem_specialist == -1) report_error("[patient.c] semget specialist", 1);

    LOG_PRINTF("|PATIENT %d| Going to %s%s...", patient_id, specialist_name,
               has_guardian ? " (with guardian)" : "");

    filled_card.mtype = filled_card.triage;
    
    if(sem_acquire(sem_specialist) == -1) {
        report_error("[patient.c] acquire specialist queue", 1);
    }

    int msgsnd_pat_specialist = msgsnd(msg_specialist, &filled_card, sizeof(struct PatientCard) - sizeof(long), 0);
    if(msgsnd_pat_specialist == -1) {
        report_error("[patient.c] msgsnd_pat_specialist", 1);
    }

    while(msgrcv(msg_specialist, &filled_card, sizeof(struct PatientCard) - sizeof(long), patient_id, 0) == -1) {
        if(errno == EINTR) continue;
        report_error("[patient.c] msgrcv_pat_specialist", 1);
    }

    LOG_PRINTF("|PATIENT %d| Treatment by %s completed, decision=%d%s", 
               patient_id, specialist_name, filled_card.sdoc_dec,
               has_guardian ? " (with guardian)" : "");

    key_t key_sem_gen_end = ftok(FTOK_PATH, ID_SEM_GEN);
    if(key_sem_gen_end != -1) {
        int sem_gen = semget(key_sem_gen_end, 1, 0600);
        if(sem_gen != -1) {
            sem_release(sem_gen);
        }
    }

    LOG_PRINTF("|PATIENT %d| Leaving hospital%s.", patient_id,
               has_guardian ? " (with guardian)" : "");
    return 0;
}