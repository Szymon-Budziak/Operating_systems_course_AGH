#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        exit(1);
    } else if (strcmp(argv[1], "ignore") == 0) {
        fprintf(stdout, "---TESTING ignore---\n");
        signal(SIGUSR1, SIG_IGN);

        fprintf(stdout, "Raising signal in main process...\n");
        raise(SIGUSR1);
        fprintf(stdout, "Calling execl...\n");
        if (execl("./child", "./child", argv[1], NULL) == -1) {
            fprintf(stdout, "execl() error\n");
            exit(1);
        }
    } else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        if (strcmp(argv[1], "mask") == 0)
            fprintf(stdout, "---TESTING mask---\n");
        else
            fprintf(stdout, "---TESTING pending---\n");


        sigset_t pending_set;
        // Emptying the mask
        sigemptyset(&pending_set);
        // Adding signal to the set
        sigaddset(&pending_set, SIGUSR1);
        //Blocking each signal in the set
        if (sigprocmask(SIG_BLOCK, &pending_set, NULL) != 0) {
            perror("Signal blocking failed\n");
        }

        fprintf(stdout, "Raising signal in main process...\n");
        raise(SIGUSR1);

        sigpending(&pending_set);
        if (sigismember(&pending_set, SIGUSR1))
            fprintf(stdout, "Signal is pending. PID: %d\n", getpid());
        else
            fprintf(stdout, "Signal is not pending. PID: %d\n", getpid());

        fprintf(stdout, "Calling execl...\n");
        if (execl("./child", "./child", argv[1], NULL) == -1) {
            fprintf(stdout, "execl() error\n");
            exit(1);
        }
    } else {
        perror("Wrong argument\n");
        exit(1);
    }
}