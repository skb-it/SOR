#include "errors.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

void report_error(const char* msg, int critical){
    perror(msg);

    char buffer[1024];
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    int len = snprintf(buffer, sizeof(buffer), "[ERROR][%s] %s: %s\n", 
                       time_str, msg, strerror(errno));

    if (len > 0) {
        const char *temp_file = "temp_error.buf";
        const char *log_file = "error_sor.txt";

        int fd_temp = creat(temp_file, 0644);
        if (fd_temp != -1) {
            write(fd_temp, buffer, len);
            close(fd_temp);
        }

        int fd_read = open(temp_file, O_RDONLY);
        char read_buf[1024];
        int read_bytes = 0;

        if (fd_read != -1) {
            read_bytes = read(fd_read, read_buf, sizeof(read_buf));
            close(fd_read);
            unlink(temp_file);
        }

        if (read_bytes > 0) {
            int fd_main = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_main != -1) {
                write(fd_main, read_buf, read_bytes);
                close(fd_main);
            }
        }
    }

    if (critical==1){
        exit(1);
    }
}
