#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int i = 0;

void clean() {
    remove("lockfile.lock");
}

void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s", message);
        fflush(stdout);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

void __aquire() {
    int fd;
    while ((fd = open("lockfile.lock", O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) {
        if (errno != EEXIST) {
            perror("Failed to acquire lock");
            exit(EXIT_FAILURE);
        }
        usleep(10); // Wait 100ms before retrying
    }
    close(fd);
}

void __release() {
    if (remove("lockfile.lock") != 0) {
        perror("Failed to release lock");
    }
}

int main(int argc, const char* argv[]) {
    atexit(clean);
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>\n", argv[0]);
        return 1;
    }

    int child_processes_count = argc - 2; // 1 for argv[argc-1] + 1 for parent
    pid_t pid;

    for (; i < child_processes_count; ++i) {
        pid = fork();
        if (pid == 0) { // Child process
            __aquire();
            write_message(argv[i+1], atoi(argv[argc - 1]));
            __release();
            exit(0);
        } else if (pid < 0) {
            perror("Fork failed");
            return 1;
        }
    }

    // Parent process
    for (int j = 0; j < child_processes_count; ++j) {
        wait(NULL); // Wait for all child processes to finish
    }

    return 0;
}
