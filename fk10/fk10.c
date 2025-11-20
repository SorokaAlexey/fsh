#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "fk10.h"

#define LOG_FILE "/tmp/fsh.log"
#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

// Function implementation for logging commands
void log_command(const char *input) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        log_file = fopen(LOG_FILE, "w");
        if (log_file == NULL) {
            perror("Error creating log file");
            return;
        }
    }
    time_t now;
    time(&now);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';
    fprintf(log_file, "[%s] [Command] %s\n", time_str, input);
    fclose(log_file);
}

// Function to clear the log file
void clear_log() {
    FILE *log_file = fopen(LOG_FILE, "w");
    if (log_file == NULL) {
        printf(RED "Error opening log file\n" RESET);
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), log_file) != NULL) {
        printf("%s", line);
    }
    fclose(log_file);
    printf(GREEN "Log file cleared\n" RESET);
}

// Function to display the log file
void show_log() {
    FILE *log_file = fopen(LOG_FILE, "r");
    if (log_file == NULL) {
        printf(RED "Error opening log file\n" RESET);
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), log_file) != NULL) {
        printf("%s", line);
    }
    fclose(log_file);
}
