#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#define QUEUE_KEY 12345
#define SHM_KEY 23456
#define SEM_KEY 34567

#define N 10 //number of patients
#define K (N/2)

#define MSG_TYPE_RED    1
#define MSG_TYPE_YELLOW 2
#define MSG_TYPE_GREEN  3
#define MSG_TYPE_REG    10

typedef struct {
    long mtype;
    pid_t patient_pid;
    int age;
    int vip; //1=true, 0=false
} PatientCard;

typedef struct {
    int waiting_patients;
    int is_second_window_open; //1=true, 0=false
    int simulation_end; //1=true, 0=false
} HospitalStatus;

#endif