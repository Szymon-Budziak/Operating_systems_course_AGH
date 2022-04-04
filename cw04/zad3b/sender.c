#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int count = 0;
int signals_number;
char *mode;
bool wait;

void handler1(int sig, siginfo_t *info, void *ucontext) {
    count++;
    wait = false;
    if (strcmp(mode, "SIGQUEUE") == 0) {
        printf("Sender received SIGUSR1 signal\n");
        printf("Sender received %d signals caught and sent back from catcher\n", info->si_value.sival_int);
    }
}

void handler2(int sig) {
    printf("Sender received SIGUSR2 signal from catcher\n");
    printf("Sender received %d SIGUSR1 signals back and should received %d signals\n", count, signals_number);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    int catcher_PID;

    if ((catcher_PID = atoi(argv[1])) < 1) {
        perror("Invalid PID catcher_PID\n");
        exit(1);
    }
    if ((signals_number = atoi(argv[2])) < 1) {
        perror("Invalid number of signals\n");
        exit(1);
    }
    mode = argv[3];
    wait = false;

    printf("Sender with PID: %d\n", getpid());

    // SIGUSR1
    static struct sigaction sig1;
    sig1.sa_sigaction = handler1;
    sig1.sa_flags = SA_SIGINFO;
    sigemptyset(&sig1.sa_mask);

    // SIGUSR2
    static struct sigaction sig2;
    sig2.sa_handler = handler2;
    sigemptyset(&sig2.sa_mask);

    if (strcmp(mode, "SIGRT") != 0) {
        if (sigaction(SIGUSR1, &sig1, NULL) == -1)
            perror("Sigaction error\n");
        if (sigaction(SIGUSR2, &sig2, NULL) == -1)
            perror("Sigaction error\n");
    } else {
        if (sigaction(SIGRTMIN + 1, &sig1, NULL) == -1)
            perror("Sigaction error\n");
        if (sigaction(SIGRTMIN + 2, &sig2, NULL) == -1)
            perror("Sigaction error\n");
    }

    // send SIGUSR1 signals
    printf("Sender sending %d SIGUSR1 signals to catcher\n", signals_number);
    for (int i = 0; i < signals_number; ++i) {
        while (wait) {}
        wait = true;
        if (strcmp(mode, "KILL") == 0)
            kill(catcher_PID, SIGUSR1);
        else if (strcmp(mode, "SIGQUEUE") == 0) {
            union sigval value;
            sigqueue(catcher_PID, SIGUSR1, value);
        } else if (strcmp(mode, "SIGRT") == 0)
            kill(catcher_PID, SIGRTMIN + 1);
        else
            printf("Wrong argument\n");
    }
    while (wait) {}

    printf("Sender sent all %d SIGUSR1 signals to catcher\n", signals_number);

    // send SIGUSR2 signal
    if (strcmp(mode, "KILL") == 0)
        kill(catcher_PID, SIGUSR2);
    else if (strcmp(mode, "SIGQUEUE") == 0) {
        union sigval value;
        sigqueue(catcher_PID, SIGUSR2, value);
    } else if (strcmp(mode, "SIGRT") == 0)
        kill(catcher_PID, SIGRTMIN + 2);
    else
        printf("Wrong argument\n");
    printf("Sender sent SIGUSR2 signal to catcher\n");
    while (1);
}