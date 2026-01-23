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
#define ID_SEM_GEN          'w'
#define ID_SEM_LOG_FILE     'x'
#define ID_SEM_STATS        'y'
#define ID_SHM_STATS        'z'
#define ID_SHM_PAT_REG      'k'
#define ID_SHM_REG_DOC      'l'
#define ID_MSG_PAT_DOC      'm'
#define ID_MSG_PAT_REG      'n'
#define ID_MSG_PAT_CARDIO   'o'
#define ID_MSG_PAT_NEURO    'p'
#define ID_MSG_PAT_EYE      'r'
#define ID_MSG_PAT_LARYNG   's'
#define ID_MSG_PAT_SURGEON  't'
#define ID_MSG_PAT_PEDIATR  'u'

#define DOC_CARDIOLOGIST  1
#define DOC_NEUROLOGIST   2
#define DOC_EYE_DOC       3
#define DOC_LARYNGOLOGIST 4
#define DOC_SURGEON       5
#define DOC_PEDIATRICIAN  6

struct Data{
    int N;
};

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

static inline void free_slot(int semget){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    while(semop(semget, &sb, 1) == -1) {
        if(errno == EINTR) {
            continue;
        }
        report_error("[common.h] semop_free_slot failed", 1);
    }
}

static inline void acquire_log_semaphore(int sem_log){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    while(semop(sem_log, &sb, 1) == -1) {
        if(errno == EINTR) continue;
        report_error("[common.h] semop_log acquire failed", 1);
    }
}

static inline void release_log_semaphore(int sem_log){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    while(semop(sem_log, &sb, 1) == -1) {
        if(errno == EINTR) continue;
        report_error("[common.h] semop_log release failed", 1);
    }
}

static inline void write_log(const char *format, ...){
    char buffer[512];
    va_list args;
    
    //FORMATING MESSAGE
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    //COLOR ASSIGNMENT
    const char *color = COLOR_RESET;
    if(strstr(buffer, "[ERROR]")) {
        color = COLOR_RED;
    } else if(strstr(buffer, "[PATIENT")) {
        color = COLOR_GREEN;
    } else if(strstr(buffer, "[GUARDIAN")) {
        color = COLOR_YELLOW;
    } else if(strstr(buffer, "[DIRECTOR]")) {
        color = COLOR_BLUE;
    } else if(strstr(buffer, "|DOCTOR") || strstr(buffer, "|REGISTRATION") || 
              strstr(buffer, "|CARDIOLOGIST") || strstr(buffer, "|NEUROLOGIST") ||
              strstr(buffer, "|LARYNGOLOGIST") || strstr(buffer, "|SURGEON") ||
              strstr(buffer, "|EYE DOCTOR") || strstr(buffer, "|PEDIATRICIAN")) {
        color = COLOR_CYAN;
    } else if(strstr(buffer, "FINAL REPORT")) {
        color = COLOR_BLUE;
    }
    
    //PRINTING WITH COLOR
    printf("%s%s%s\n", color, buffer, COLOR_RESET);
    fflush(stdout);
    
    //SEMAPHORE FOR LOG FILE
    key_t log_key = ftok(FTOK_PATH, ID_SEM_LOG_FILE);
    int sem_id = semget(log_key, 1, 0600);
    if(sem_id == -1) {
        return;
    }
    
    acquire_log_semaphore(sem_id);
    
    int fd = open(LOG_FILE, O_WRONLY | O_APPEND);
    if(fd != -1) {
        write(fd, buffer, strlen(buffer));
        write(fd, "\n", 1);
        close(fd);
    }
    
    release_log_semaphore(sem_id);
}

#define LOG_PRINTF(fmt, ...) write_log(fmt, ##__VA_ARGS__)

static inline void stats_acquire_lock(int sem_id) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    
    while(semop(sem_id, &sb, 1) == -1) {
        if(errno == EINTR) continue;
        return;
    }
}

static inline void stats_release_lock(int sem_id) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    
    while(semop(sem_id, &sb, 1) == -1) {
        if(errno == EINTR) continue;
        return;
    }
}

static inline void increment_doctor_count(int doctor_type){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    
    switch(doctor_type){
        case DOC_CARDIOLOGIST: stats->cardiologist_count++; LOG_PRINTF("[STATS] Cardiologist count now: %d", stats->cardiologist_count); break;
        case DOC_NEUROLOGIST: stats->neurologist_count++; LOG_PRINTF("[STATS] Neurologist count now: %d", stats->neurologist_count); break;
        case DOC_EYE_DOC: stats->eye_doctor_count++; LOG_PRINTF("[STATS] Eye doctor count now: %d", stats->eye_doctor_count); break;
        case DOC_LARYNGOLOGIST: stats->laryngologist_count++; LOG_PRINTF("[STATS] Laryngologist count now: %d", stats->laryngologist_count); break;
        case DOC_SURGEON: stats->surgeon_count++; LOG_PRINTF("[STATS] Surgeon count now: %d", stats->surgeon_count); break;
        case DOC_PEDIATRICIAN: stats->pediatrician_count++; LOG_PRINTF("[STATS] Pediatrician count now: %d", stats->pediatrician_count); break;
    }
    
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_pc_doctor_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->pc_doctor_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_total_patients(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->total_patients++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_sent_home_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->sent_home++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_cardiologist_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->cardiologist_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_neurologist_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->neurologist_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_eye_doctor_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->eye_doctor_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_laryngologist_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->laryngologist_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_surgeon_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->surgeon_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

static inline void increment_pediatrician_count(){
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return;
    
    int shm_stats = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(shm_stats == -1) return;
    
    struct PatientStats *stats = shmat(shm_stats, NULL, 0);
    if(stats == (void *)-1) return;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) goto detach;
    
    int sem_stats = semget(key_sem_stats, 1, 0600);
    if(sem_stats == -1) goto detach;
    
    stats_acquire_lock(sem_stats);
    stats->pediatrician_count++;
    stats_release_lock(sem_stats);
    
detach:
    shmdt(stats);
}

#define SHM_SIZE_INT sizeof(int)
#define SHM_SIZE_CARD sizeof(struct PatientCard)
#define SHM_SIZE_INFO sizeof(struct PatientInfo)
#define SHM_SIZE_STATS sizeof(struct PatientStats)


#endif