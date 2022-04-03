#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int count = 0;
char *mode;

void handler1(int sig) {
    count++;
}

void handler2(int sig, siginfo_t *info, void *ucontext) {
    printf("Catcher received SIGUSR2. Now it is going to send back %d SIGUSR1 signals\n", count);

    pid_t PID_sender;
    PID_sender = info->si_pid;

    // send SIGUSR1 signals
    for (int i = 0; i < count; i++) {
        printf("%d SIGUSR1 signal sent back\n", i + 1);
        if (strcmp(mode, "KILL") == 0) {
            kill(PID_sender, SIGUSR1);
        } else if (strcmp(mode, "SIGQUEUE") == 0) {
            union sigval value;
            value.sival_int = i;
            sigqueue(PID_sender, SIGUSR1, value);
        } else if (strcmp(mode, "SIGRT") == 0) {
            kill(PID_sender, SIGRTMIN + 1);
        } else
            printf("Wrong argument\n");
    }

    // send SIGUSR2 signals
    if (strcmp(mode, "KILL") == 0) {
        kill(PID_sender, SIGUSR2);
    } else if (strcmp(mode, "SIGQUEUE") == 0) {
        union sigval value;
        sigqueue(PID_sender, SIGUSR2, value);
    } else if (strcmp(mode, "SIGRT") == 0) {
        kill(PID_sender, SIGRTMIN + 2);
    } else
        printf("Wrong argument\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    mode = argv[1];
    printf("Catcher with PID: %d waiting for SIGUSR1\n", getpid());

    // SIGUSR1
    static struct sigaction sig1;
    sig1.sa_handler = handler1;
    sigemptyset(&sig1.sa_mask);

    // SIGUSR2
    static struct sigaction sig2;
    sig2.sa_sigaction = handler2;
    sig2.sa_flags = SA_SIGINFO;
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

    sigset_t signal_set;
    sigfillset(&signal_set);
    sigdelset(&signal_set, SIGUSR1);
    sigdelset(&signal_set, SIGUSR2);
    if (strcmp(mode, "SIGRT") == 0) {
        sigdelset(&signal_set, SIGRTMIN + 1);
        sigdelset(&signal_set, SIGRTMIN + 2);
    }
    sigprocmask(SIG_SETMASK, &signal_set, NULL);
    while (1) {
        sigsuspend(&signal_set);
    }
}