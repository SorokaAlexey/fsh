// fsh.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include "fk10/fk10.h"
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

// Check if the command is a built-in command
bool is_builtin(char **args) {
    if (args == NULL || args[0] == NULL) {
        return false;
    }

    // List of single-word built-in commands
    const char *single_word_builtins[] = {"exit", "color", "flower", "cwd", "touch", "cd", "log", "ls", NULL};
    for (int i = 0; single_word_builtins[i] != NULL; i++) {
        if (strcmp(args[0], single_word_builtins[i]) == 0) {
            return true;
        }
    }

    return false;
}

// Check if the command is a multi-word built-in command
bool is_multiword_builtin(char **args) {
    if (args == NULL || args[0] == NULL || args[1] == NULL) {
        return false;
    }

    if (strcmp(args[0], "log") == 0 && strcmp(args[1], "clear") == 0) {
        return true;
    }

    return false;
}

// Execute built-in commands
void execute_builtin(char **args) {
    if (args == NULL || args[0] == NULL) {
        return;
    }

    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(args[0], "color") == 0) {
        printf(GREEN);
    }
    else if (strcmp(args[0], "flower") == 0) {
        printf(FLOWER);
    }
    else if (strcmp(args[0], "cwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf(GREEN"%s\n"RESET, cwd);
        } else {
            perror(RED "Error getting current directory" RESET);
        }
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(args[1]) != 0) {
                perror(RED "Error changing directory" RESET);
            }
        }
    }
    else if (strcmp(args[0], "log") == 0) {
        if (args[1] != NULL && strcmp(args[1], "clear") == 0) {
            clear_log();
        } else {
            show_log();
        }
    }
    else if (strcmp(args[0], "touch") == 0) {
        if (args[1] == NULL) {
            printf(RED "Usage: touch <filename>\n" RESET);
        } else {
            int fd = open(args[1], O_WRONLY | O_CREAT, 0644);
            if (fd == -1) {
                perror(RED "Error creating file" RESET);
            } else {
                close(fd);
            }
        }
    }
    else if (strcmp(args[0], "ls") == 0) {
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

// Execute a command
void execute_command(char **args) {
    if (args == NULL || args[0] == NULL) {
        return;
    }

    if (is_builtin(args) || is_multiword_builtin(args)) {
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

// Parse input into arguments
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

// Main loop of the shell
void loop() {
    char input[MAX_INPUT_SIZE];
    printf(ASCII_ART);
    while (1) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf(BLUE"%s~> "RESET, cwd);
        } else {
            perror(RED "Error getting current directory" RESET);
            printf(BLUE"~> "RESET);
        }
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        log_command(input);
        char **args = parse_input(input);
        if (args != NULL && args[0] != NULL) {
            execute_command(args);
        }
        free(args);
    }
}

// Main function
int main() {
    loop();
    return EXIT_SUCCESS;
}
