#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handle_signal(int signum) {
    fprintf(stdout, "Received signal %d. PID: %d, PPID: %d\n", signum, getpid(), getppid());
}

void raise_signal_and_fork() {
    fprintf(stdout, "Raising signal in main process...\n");
    raise(SIGUSR1);
    if (fork() == 0) {
        fprintf(stdout, "Raising signal in child process...\n");
        raise(SIGUSR1);
    }
    wait(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        exit(1);
    } else if (strcmp(argv[1], "ignore") == 0) {
        fprintf(stdout, "---TESTING ignore---\n");
        signal(SIGUSR1, SIG_IGN);

        raise_signal_and_fork();
    } else if (strcmp(argv[1], "handler") == 0) {
        fprintf(stdout, "---TESTING handler---\n");
        signal(SIGUSR1, handle_signal);

        raise_signal_and_fork();
    } else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        if (strcmp(argv[1], "mask") == 0)
            fprintf(stdout, "---TESTING mask---\n");
        else
            fprintf(stdout, "---TESTING pending---\n");

        sigset_t signal_set;
        // Emptying the mask
        sigemptyset(&signal_set);
        // Adding signal to the set
        sigaddset(&signal_set, SIGUSR1);
        // Blocking each signal in the set
        if (sigprocmask(SIG_BLOCK, &signal_set, NULL) != 0) {
            perror("Signal blocking failed\n");
        }

        fprintf(stdout, "Raising signal in main process...\n");
        raise(SIGUSR1);

        sigpending(&signal_set);
        if (sigismember(&signal_set, SIGUSR1))
            fprintf(stdout, "Signal is pending. PID: %d\n", getpid());
        else
            fprintf(stdout, "Signal is not pending. PID: %d\n", getpid());

        if (fork() == 0) {
            if (strcmp(argv[1], "mask") == 0) {
                fprintf(stdout, "Raising signal in child process...\n");
                raise(SIGUSR1);
            }
            sigpending(&signal_set);
            if (sigismember(&signal_set, SIGUSR1))
                fprintf(stdout, "Signal is pending in child process. PID: %d\n", getpid());
            else
                fprintf(stdout, "Signal is not pending in child process. PID: %d\n", getpid());
        }
        wait(NULL);
    } else {
        perror("Wrong argument\n");
        exit(1);
    }
}