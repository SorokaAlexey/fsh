#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "config.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

bool is_builtin(char *command) {
    const char *builtins[] = {"exit", "color", "flower", "cd", "ls", NULL};
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(command, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

void execute_builtin(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else if (strcmp(args[0], "color") == 0) {
        printf(GREEN);
    } else if (strcmp(args[0], "flower") == 0) {
        printf(FLOWER);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            chdir(getenv("HOME")); // Change to home directory
        }
    } else if (strcmp(args[0], "ls") == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execvp("ls", args);
            perror(RED "Error executing 'ls' command" RESET);
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror(RED "Error creating process" RESET);
        } else {
            int status;
            waitpid(pid, &status, WUNTRACED);
        }
    }
}

void execute_command(char **args) {
    if (is_builtin(args[0])) {
        execute_builtin(args);
        return;
    }
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            printf(RED "Error: command '%s' not found\n" RESET, args[0]);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror(RED "Error creating process" RESET);
    } else {
        int status;
        waitpid(pid, &status, WUNTRACED);
    }
}

char **parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
    if (!args) {
        perror(RED "Error allocating memory" RESET);
        exit(EXIT_FAILURE);
    }
    char *token = strtok(input, " \t\n");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return args;
}

void loop() {
    char input[MAX_INPUT_SIZE];
    printf(ASCII_ART);
    while (1) {
        printf("~>");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        char **args = parse_input(input);
        if (args[0] != NULL) {
            execute_command(args);
        }
        free(args);
    }
}

int main() {
    loop();
    return EXIT_SUCCESS;
}
