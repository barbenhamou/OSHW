#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/fcntl.h>

int main(int argc, const char* argv[]) {

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
        return 1;
    }

    int fdout = open("output.txt", O_CREAT | O_RDWR, 0666), status;

    if (fdout < 0) {
        perror("Cretaing file failed");
        exit(-1);
    }

    pid_t pid1 = fork(), pid2;
    if (pid1 < 0) {
        perror("fork failed");
        exit(-1);
    }

    if (pid1 == 0) {
        for (int i = 0; i < atoi(argv[4]); ++i) {
            if (write(fdout, argv[2], strlen(argv[2])) == -1) {
                perror("write failed");
                exit(-1);
            }
        }
    } else {

        pid2 = fork();

        sleep(1);

        if (pid2 < 0) {
            perror("fork failed");
            exit(-1);
        }

        if (pid2 == 0) {
            //child2
            for (int i = 0; i < atoi(argv[4]); ++i) {
                if (write(fdout, argv[3], strlen(argv[3])) == -1) {
                    perror("write failed");
                    exit(-1);
                }
            }
        } else {
            if (waitpid(pid2, &status, 0) == -1) {
                    perror("waitpid failed");
                    exit(-1);
            }
            for (int i = 0; i < atoi(argv[4]); ++i) {
                if (write(fdout, argv[1], strlen(argv[1])) == -1) {
                    perror("write failed");
                    exit(-1);
                }
            }
        }
    }
    

    return 0;
}
