#include "common.h"

int safe_semop(int semid, struct sembuf *sops, size_t nsops) {
    while(semop(semid, sops, nsops) == -1) {
        if(errno == EINTR) {
            continue;
        }
        return -1;
    }
    return 0;
}

int sem_acquire(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    return safe_semop(semid, &sb, 1);
}

int sem_release(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    return safe_semop(semid, &sb, 1);
}

int producer_wait_slot(int semid, volatile sig_atomic_t *interrupt_flag) {
    struct sembuf sb = {0, -1, SEM_UNDO};
    while(semop(semid, &sb, 1) == -1) {
        if(errno == EINTR) {
            if(interrupt_flag && *interrupt_flag) {
                return 1;
            }
            continue;
        }
        return -1;
    }
    return 0;
}

int producer_signal_data(int semid) {
    struct sembuf sb;
    sb.sem_num = 1;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    return safe_semop(semid, &sb, 1);
}

int consumer_wait_data(int semid, volatile sig_atomic_t *interrupt_flag) {
    struct sembuf sb = {1, -1, SEM_UNDO};
    while(semop(semid, &sb, 1) == -1) {
        if(errno == EINTR) {
            if(interrupt_flag && *interrupt_flag) {
                return 1;
            }
            continue;
        }
        return -1;
    }
    return 0;
}

int consumer_signal_slot(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    return safe_semop(semid, &sb, 1);
}

void write_log(const char *format, ...) {
    char buffer[512];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    const char *color = COLOR_RESET;
    if(strstr(buffer, "[ERROR]")) {
        color = COLOR_RED;
    } else if(strstr(buffer, "|PATIENT") || strstr(buffer, "[PATIENT")) {
        color = COLOR_GREEN;
    } else if(strstr(buffer, "|GUARDIAN") || strstr(buffer, "[GUARDIAN")) {
        color = COLOR_YELLOW;
    } else if(strstr(buffer, "[DIRECTOR]")) {
        color = COLOR_BLUE;
    } else if(strstr(buffer, "|DOCTOR") || strstr(buffer, "|REGISTRATION") || 
              strstr(buffer, "|CARDIOLOGIST") || strstr(buffer, "|NEUROLOGIST") ||
              strstr(buffer, "|LARYNGOLOGIST") || strstr(buffer, "|SURGEON") ||
              strstr(buffer, "|EYE DOCTOR") || strstr(buffer, "|PEDIATRICIAN") ||
              strstr(buffer, "[GENERATOR") || strstr(buffer, "[REAPER")) {
        color = COLOR_CYAN;
    } else if(strstr(buffer, "FINAL REPORT")) {
        color = COLOR_BLUE;
    }
    
    printf("%s%s%s\n", color, buffer, COLOR_RESET);
    fflush(stdout);
    
    key_t log_key = ftok(FTOK_PATH, ID_SEM_LOG_FILE);
    int sem_id = semget(log_key, 1, 0600);
    if(sem_id == -1) {
        return;
    }
    
    sem_acquire(sem_id);
    
    int fd = open(LOG_FILE, O_WRONLY | O_APPEND);
    if(fd != -1) {
        write(fd, buffer, strlen(buffer));
        write(fd, "\n", 1);
        close(fd);
    }
    
    sem_release(sem_id);
}

struct PatientStats* stats_attach(int *shm_id, int *sem_id) {
    key_t key_stats = ftok(FTOK_PATH, ID_SHM_STATS);
    if(key_stats == -1) return NULL;
    
    *shm_id = shmget(key_stats, sizeof(struct PatientStats), 0600);
    if(*shm_id == -1) return NULL;
    
    struct PatientStats *stats = shmat(*shm_id, NULL, 0);
    if(stats == (void *)-1) return NULL;
    
    key_t key_sem_stats = ftok(FTOK_PATH, ID_SEM_STATS);
    if(key_sem_stats == -1) {
        shmdt(stats);
        return NULL;
    }
    
    *sem_id = semget(key_sem_stats, 1, 0600);
    if(*sem_id == -1) {
        shmdt(stats);
        return NULL;
    }
    
    return stats;
}

void increment_doctor_count(int doctor_type) {
    int shm_id, sem_id;
    struct PatientStats *stats = stats_attach(&shm_id, &sem_id);
    if(!stats) return;
    
    sem_acquire(sem_id);
    
    switch(doctor_type) {
        case DOC_CARDIOLOGIST: stats->cardiologist_count++; break;
        case DOC_NEUROLOGIST: stats->neurologist_count++; break;
        case DOC_EYE_DOC: stats->eye_doctor_count++; break;
        case DOC_LARYNGOLOGIST: stats->laryngologist_count++; break;
        case DOC_SURGEON: stats->surgeon_count++; break;
        case DOC_PEDIATRICIAN: stats->pediatrician_count++; break;
    }
    
    sem_release(sem_id);
    shmdt(stats);
}

void increment_pc_doctor_count(void) {
    int shm_id, sem_id;
    struct PatientStats *stats = stats_attach(&shm_id, &sem_id);
    if(!stats) return;
    
    sem_acquire(sem_id);
    stats->pc_doctor_count++;
    sem_release(sem_id);
    shmdt(stats);
}

void increment_total_patients(void) {
    int shm_id, sem_id;
    struct PatientStats *stats = stats_attach(&shm_id, &sem_id);
    if(!stats) return;
    
    sem_acquire(sem_id);
    stats->total_patients++;
    sem_release(sem_id);
    shmdt(stats);
}

void increment_sent_home_count(void) {
    int shm_id, sem_id;
    struct PatientStats *stats = stats_attach(&shm_id, &sem_id);
    if(!stats) return;
    
    sem_acquire(sem_id);
    stats->sent_home++;
    sem_release(sem_id);
    shmdt(stats);
}