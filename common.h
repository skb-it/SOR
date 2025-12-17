#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define TRIAGE_RED    1
#define TRIAGE_YELLOW 2
#define TRIAGE_GREEN  3

#define FTOK_PATH "."

#define ID_SEM_WAITING_ROOM 'S'
#define ID_MSG_REGISTRATION 'R'
#define ID_MSG_PATIENT      'P'
#define ID_MSG_DOCTOR       'D'
#define ID_SHM              'M'


#define SHM_SIZE sizeof(int)

struct Message {
    long mtype;
    pid_t patient_id;
    int age;
    char description[100];
};

#endif