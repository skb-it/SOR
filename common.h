#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#define VIP    1
#define COMMON 2

#define TRIAGE_RED    1
#define TRIAGE_YELLOW 2
#define TRIAGE_GREEN  3
#define SENT_HOME     4

#define FTOK_PATH "."

#define ID_SEM_WAITING_ROOM 'A'
#define ID_SHM_N            'B'
#define ID_SHM_PAT_REG      'C'
#define ID_SHM_REG_DOC      'D'
#define ID_SHM_CARD         'E'
#define ID_MSG_PAT_REG      'F'

#define DOC_CARDIOLOGIST  'G'
#define DOC_NEUROLOGIST   'H'
#define DOC_EYE_DOC       'I'
#define DOC_LARYNGOLOGIST 'J'
#define DOC_SURGEON       'K'
#define DOC_PEDIATRICIAN  'L'


struct Data{
    int N;
};

struct PatientInfo {
    long mtype;
    pid_t patient_id;
    int age;
    int is_guardian; // 0 = NO, 1 = YES
};

struct MsgBuffer {
    long mtype;
    int age;
    pid_t patient_id;
    int is_guardian;
};

struct PatientCard {
    long mtype;
    int is_vip;
    pid_t patient_id;
    int age;   
    int is_guardian;
    int triage;
    int flag;
};

#define SHM_SIZE_INT sizeof(int)
#define SHM_SIZE_CARD sizeof(struct PatientCard)
#define SHM_SIZE_INFO sizeof(struct PatientInfo)


#endif