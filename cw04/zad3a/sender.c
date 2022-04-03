#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int count = 0;
int signals_number;
char *mode;

void handler1(int sig, siginfo_t *info, void *ucontext) {
    count++;
    if (strcmp(mode, "SIGQUEUE") == 0) {
        printf("SIGUSR1 handler caught signal\n");
        printf("Number of signals caught and sent back from catcher: %d\n", info->si_value.sival_int);
    }
}

void handler2(int sig, siginfo_t *info, void *ucontext) {
    printf("***All signals sent back***\n");
    printf("All SIGUSR1 signals caught by sender: %d and should be %d\n", count, signals_number);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    int PID_catcher;

    if ((PID_catcher = atoi(argv[1])) < 1) {
        perror("Invalid PID catcher\n");
        exit(1);
    }
    if ((signals_number = atoi(argv[2])) < 1) {
        perror("Invalid number of signals\n");
        exit(1);
    }
    mode = argv[3];

    printf("Sender with PID: %d\n", getpid());


    // SIGUSR1
    static struct sigaction sig1;
    sig1.sa_sigaction = handler1;
    sig1.sa_flags = SA_SIGINFO;
    sigemptyset(&sig1.sa_mask);

    // SIGUSR2
    static struct sigaction sig2;
    sig2.sa_sigaction = handler2;
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
    printf("%d SIGUSR1 signals will be sent\n", signals_number);
    for (int i = 0; i < signals_number; ++i) {
        if (strcmp(mode, "KILL") == 0) {
            kill(PID_catcher, SIGUSR1);
        } else if (strcmp(mode, "SIGQUEUE") == 0) {
            union sigval value;
            sigqueue(PID_catcher, SIGUSR1, value);
        } else if (strcmp(mode, "SIGRT") == 0) {
            kill(PID_catcher, SIGRTMIN + 1);
        } else
            printf("Wrong argument\n");
    }

    printf("All %d SIGUSR1 signals sent\n", signals_number);

    // send SIGUSR2 signals
    if (strcmp(mode, "KILL") == 0) {
        kill(PID_catcher, SIGUSR2);
    } else if (strcmp(mode, "SIGQUEUE") == 0) {
        union sigval value;
        sigqueue(PID_catcher, SIGUSR2, value);
    } else if (strcmp(mode, "SIGRT") == 0) {
        kill(PID_catcher, SIGRTMIN + 2);
    } else
        printf("Wrong argument\n");

    while (1);
}