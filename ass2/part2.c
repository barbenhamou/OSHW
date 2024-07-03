#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/fcntl.h>
#include <errno.h>

int i = 0;

void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

void __aquire() {
    errno = 0;
    int fd = open("lockfile.lock", O_CREAT | O_EXCL, 0666);
    if (fd == -1 && errno != EEXIST) {
        perror("open failed");
        exit(-1);
    }
    while (errno == EEXIST) {
        errno = 0;
        usleep((rand() % 100) * 10000);
        fd = open("lockfile.lock", O_CREAT | O_EXCL, 0666);
        if (fd == -1 && errno != EEXIST) {
            perror("open failed");
            exit(-1);
        }
    }
}

void __release() {
    remove("lockfile.lock");
}

int main(int argc, const char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
        return 1;
    }

    int child_processes_count = argc - 3;
    pid_t pid;

    for (; i <= child_processes_count; ++i) {
        if ((pid = fork()) < 0) {
            perror("fork failed");
            exit(-1);
        }
        if (pid == 0) {
            srand(getpid());
            __aquire();
            write_message(argv[i+1], atoi(argv[argc - 1]));
            __release();
            exit(1);
        } 
    } 

    for (int j = 0; j < child_processes_count; ++j) { 
        wait(NULL);
    }

    remove("lockfile.lock");
    return 0;
}

 