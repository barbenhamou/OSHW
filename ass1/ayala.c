#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

char history[100][101];
int end = 0;

    void history_func(){
    
    int start = (end + 1)%100;
    for( int i = 0; i < 100; i++){
        if (history[start][0] == '\0'){
            start = (start +1)%100;
            continue;
        }
        printf("%s\n", history[start]);
        start = (start +1)%100;
    }
    }

    void pwd_func(){
        
    char cwd[101] = {'\0'}; 

    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() failed");
    }
    }

    void exit_func(){
        exit(1);    
    }

int main(int argc, const char *argv[]) {
    char path_value[1024];
    // Get the current PATH environment variable
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


    while( 1 ){

    char str[101] = {'\0'};
    printf("$ ");
    fflush(stdout);

     if (scanf("%101[^\n]", str) != 1) {
        perror("scanf failed");
        return 1;
    }
    getchar();

    char command[101] = {0};
    int i = 0;

    while( str[i] != ' '  && str[i] != '\0' && i < 100){
        command[i] = str[i];
        i++;
    }
    command[i] = '\0';

    strncpy(history[end], str, 100); 
    end = (end + 1)%100;


    if (strcmp(command, "history") == 0) {
        history_func();
    } else if (strcmp(command, "pwd") == 0) {
        pwd_func();
    } else if (strcmp(command, "exit") == 0) {
        exit_func();
    } else if (strcmp(command, "cd") == 0) {
        if (chdir(str + i + 1) != 0) {
            perror("chdir failed");
            return 1;
        }
    } else {

        char* words[101] = {0};
        int wordCount = 0;

        char *token = strtok(str, " ");
            while (token != NULL) {
                words[wordCount] = token;
                wordCount++;
                token = strtok(NULL, " ");
            }
        words[wordCount] = NULL;

        int stat,waited,ret_code;
        pid_t pid;
        pid = fork();
        if (pid == 0) 
        {  /* Child */ 

            ret_code = execvp(words[0],words);  
            if (ret_code == -1) 
            {
                perror("exec failed ");
                return -1;
            }
        }
        else if (pid > 0) {
            // Parent process
            if (wait(&stat) == -1) {
                perror("wait failed");
            }
        } else {
            perror("fork failed");
            return 1;
    }
    }
}

return 0;
}