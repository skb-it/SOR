#include "errors.h"
#include "common.h"


void report_error(const char* msg, int critical){
    perror(msg);

    FILE *log_file = fopen("log_sor.txt", "a");
    if (log_file != NULL) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0';

        fprintf(log_file, "[ERROR][%s] %s: %s\n", time_str, msg, strerror(errno));
        fclose(log_file);
    }

    if (critical) {
        exit(1);
    }
}
