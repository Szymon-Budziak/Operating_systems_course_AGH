#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }
    long n;
    char *p;
    n = strtol(argv[1], &p, 10);
    if (*p != '\0') {
        fprintf(stderr, "Invalid parameter, which is not a number\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            fprintf(stdout, "Child process %d from %d\n", getpid(), getppid());
            exit(0);
        }
    }
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
}