#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "errors.h"

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

#define VIP    1
#define COMMON 2

#define TRIAGE_RED    1
#define TRIAGE_YELLOW 2
#define TRIAGE_GREEN  3

#define SENT_HOME     4

#define SENT_TO_WARD  1
#define OTHER_S_HOSP  2

#define FTOK_PATH "."
#define LOG_FILE "log_sor.txt"

#define ID_SEM_WAITING_ROOM 'a'
#define ID_SEM_DOC          'b'
#define ID_SEM_MSG_PAT_DOC  'c'
#define ID_SEM_MSG_CARDIO   'd'
#define ID_SEM_MSG_EYEDOC   'e'
#define ID_SEM_MSG_LARYNG   'f'
#define ID_SEM_MSG_NEURO    'g'
#define ID_SEM_MSG_PEDIATR  'h'
#define ID_SEM_MSG_SURGEON  'i'
#define ID_SEM_MSG_REG      'j'
#define ID_SEM_GEN          'k'
#define ID_SEM_LOG_FILE     'l'
#define ID_SEM_STATS        'm'
#define ID_SHM_STATS        'n'
#define ID_SHM_PAT_REG      'o'
#define ID_SHM_REG_DOC      'p'
#define ID_MSG_PAT_DOC      'r'
#define ID_MSG_PAT_REG      's'
#define ID_MSG_PAT_CARDIO   't'
#define ID_MSG_PAT_NEURO    'u'
#define ID_MSG_PAT_EYE      'w'
#define ID_MSG_PAT_LARYNG   'x'
#define ID_MSG_PAT_SURGEON  'y'
#define ID_MSG_PAT_PEDIATR  'z'

#define DOC_CARDIOLOGIST  1
#define DOC_NEUROLOGIST   2
#define DOC_EYE_DOC       3
#define DOC_LARYNGOLOGIST 4
#define DOC_SURGEON       5
#define DOC_PEDIATRICIAN  6

struct PatientStats {
    int total_patients;
    int cardiologist_count;
    int neurologist_count;
    int eye_doctor_count;
    int laryngologist_count;
    int surgeon_count;
    int pediatrician_count;
    int pc_doctor_count;
    int sent_home;
};

struct Msg {
    long mtype;
    int age;
    pid_t patient_id;
    int is_guardian;
};

struct PatientCard {
    long mtype;
    pid_t patient_id;
    int age;
    int is_guardian;
    int triage;
    int sdoc;
    int sdoc_dec;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int safe_semop(int semid, struct sembuf *sops, size_t nsops);
int sem_acquire(int semid);
int sem_release(int semid);
int producer_wait_slot(int semid, volatile sig_atomic_t *interrupt_flag);
int producer_signal_data(int semid);
int consumer_wait_data(int semid, volatile sig_atomic_t *interrupt_flag);
int consumer_signal_slot(int semid);

void write_log(const char *format, ...);
#define LOG_PRINTF(fmt, ...) write_log(fmt, ##__VA_ARGS__)

struct PatientStats* stats_attach(int *shm_id, int *sem_id);
void increment_doctor_count(int doctor_type);
void increment_pc_doctor_count(void);
void increment_total_patients(void);
void increment_sent_home_count(void);

#define SHM_SIZE_CARD sizeof(struct PatientCard)

#endif