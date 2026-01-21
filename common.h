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

#define SENT_TO_WARD  1
#define OTHER_S_HOSP  2

#define FTOK_PATH "."

#define ID_SEM_WAITING_ROOM 'a'
#define ID_SEM_DOC          'b'
#define ID_SEM_MSG_CARDIO   'n'
#define ID_SEM_MSG_EYEDOC   'o'
#define ID_SEM_MSG_LARYNG   'p'
#define ID_SEM_MSG_NEURO    'r'
#define ID_SEM_MSG_PEDIATR  's'
#define ID_SEM_MSG_SURGEON  't'
#define ID_SEM_MSG_REG      'c'
#define ID_SHM_PAT_REG      'd'
#define ID_SHM_REG_DOC      'e'
#define ID_MSG_PAT_DOC      'f' //consider something else
#define ID_MSG_PAT_REG      'g'
#define ID_MSG_PAT_CARDIO   'h'
#define ID_MSG_PAT_NEURO    'i'
#define ID_MSG_PAT_EYE      'j'
#define ID_MSG_PAT_LARYNG   'k'
#define ID_MSG_PAT_SURGEON  'l'
#define ID_MSG_PAT_PEDIATR  'm'

#define DOC_CARDIOLOGIST  1
#define DOC_NEUROLOGIST   2
#define DOC_EYE_DOC       3
#define DOC_LARYNGOLOGIST 4
#define DOC_SURGEON       5
#define DOC_PEDIATRICIAN  6


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
    int sdoc;
    int sdoc_dec;
    int flag;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define SHM_SIZE_INT sizeof(int)
#define SHM_SIZE_CARD sizeof(struct PatientCard)
#define SHM_SIZE_INFO sizeof(struct PatientInfo)


#endif