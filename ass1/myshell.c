#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_AMOUNT 100

char* custom_commands[] = {"cd", "history", "exit", "pwd"};

char* history[MAX_AMOUNT];

int head = 0;

void update_history(char* args) {
    history[head] = (char*)malloc(strlen(args) + 1);
    if (history[head] == NULL) {
        perror("malloc failed");
        exit(0);
    }

    strcpy(history[head], args);
    head++;
}

void free_history() {
    for (int k = 0; k < head; ++k) {
        free(history[k]);
    }
}

void process_custom(char* command, char* args[], int argc) {

    if (!strcmp(command, "pwd")) {
        char current_working_directory[1024];
        getcwd(current_working_directory, sizeof(current_working_directory));
        if (current_working_directory != NULL) {
            printf("%s\n", current_working_directory);
        } else {
            perror("getcwd failed");
        }

    } else if (!strcmp(command, "history")) {
        for (int j = 0; j < head; ++j) {
            puts(history[j]);
        }

    } else if (!strcmp(command, "cd")) {
        if (chdir(args[1]) != 0) {
            perror("chdir failed");
        }

    } else if (!strcmp(command, "exit")) {
        exit(0);
    }
}

void process_non_custom(char* command, char* args[], int argc) {
    int status;
    pid_t pid, wpid;
    pid = fork();
    if (pid == 0) {

        if (execvp(args[0], args) == -1) {
            perror("exec failed");
        }
        exit(3);

    } else if (pid < 0) {
        perror("fork failed");

    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1) {
                perror("waitpid failed");
            }
        } while (!(WIFEXITED(status) || WIFSIGNALED(status)));
    }
}

void process_input(char* args) {
    char* command = strtok(args, " ");
    char* arguments[MAX_COMMAND_LENGTH + 1];
    char* check = (char*)malloc(strlen(command + 1));
    if (check == NULL) {
        perror("malloc failed");
        exit(0);
    }

    arguments[0] = (char*)malloc(strlen(command) + 1);
    if (arguments[0] == NULL) {
            perror("malloc failed");
            exit(0);
    }
    strcpy(arguments[0], command);

    int i = 1;

    while ((check = strtok(NULL, " ")) != NULL) {
        arguments[i] = (char*)malloc(strlen(check) + 1);
        if (arguments[i] == NULL) {
            perror("malloc failed");
            exit(0);
        }
        strcpy(arguments[i], check);
        ++i;
    } 

    free(check);

    arguments[i] = NULL;
        
    if (!strcmp(command, custom_commands[0]) 
    || !strcmp(command, custom_commands[1]) 
    || !strcmp(command, custom_commands[2]) 
    || !strcmp(command, custom_commands[3])) {
        process_custom(command, arguments, i);

    } else {
        process_non_custom(command, arguments, i);
    }

    for (int j = 0; j < i; ++j) {
        free(arguments[j]);
    }
}

int main(int argc, char* argv[], char** envp) {
    char path_value[1024];  // Buffer for constructing new PATH value
    path_value[0] = '\0';   // Start with an empty string

    // Concatenate all directories from argv to path_value
    for (int i = 1; i < argc; ++i) {
        strcat(path_value, argv[i]);
        strcat(path_value, ":");
    }

    // Get current PATH from environment
    char* env_path = getenv("PATH");
    if (env_path == NULL) {
        perror("getenv failed");
        return 1;
    }

    // Concatenate existing PATH to path_value
    strcat(path_value, env_path);

    // Set new PATH value
    if (setenv("PATH", path_value, 1) == -1) {
        perror("setenv failed");
        return 1;
    }


    atexit(free_history);
    char input[MAX_COMMAND_LENGTH];
    while (1) {
        printf("$ ");
        scanf(" %[^\n]", input);
        update_history(input);
        process_input(input);
    }

    return 0;
}
