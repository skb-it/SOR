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

#define TRIAGE_RED 1
#define TRIAGE_YELLOW 2
#define TRIAGE_GREEN 3

#define KEY_SEM_WAITING_ROOM 1111
#define KEY_MSG_REGISTRATION 2222
#define KEY_MSG_DOCTOR 3333
#define KEY_SHM 4444

#define SHM_SIZE sizeof(int)

typedef struct {
    long mtype;
    pid_t patientID;
    int age;
} PatientMsg;


#endif