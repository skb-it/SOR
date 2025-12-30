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

#define COMMON 2
#define VIP    1


#define TRIAGE_RED    1
#define TRIAGE_YELLOW 2
#define TRIAGE_GREEN  3
#define SENT_HOME     4

#define FTOK_PATH "."

#define ID_SEM_WAITING_ROOM 'S'
#define ID_SHM_PAT_REG      'P'
#define ID_MSG_REG_DOC      'R'

#define DOC_CARDIOLOGIST  'C'
#define DOC_NEUROLOGIST   'N'
#define DOC_EYE_DOC       'E'
#define DOC_LARYNGOLOGIST 'L'
#define DOC_SURGEON       'S'
#define DOC_PEDIATRICIAN  'K'

#define HOME     0
#define WARD     1
#define OTHER    2

#define SHM_SIZE sizeof(int)

struct Message {
    long mtype;
    pid_t patient_id;
    int age;
    int is_vip;   
    int is_guardian;
    int triage_color;  
};



#endif