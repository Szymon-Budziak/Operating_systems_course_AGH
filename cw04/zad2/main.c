#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Entering SA_SIGINFO handler\n");
    printf("Signal number from sig: %d, from siginfo %d\n", sig, info->si_signo);
    printf("PID: %d, PPID: %d\n", info->si_pid, getppid());
    printf("Real user ID of sending a process: %d\n", info->si_uid);
    printf("System time consumed: %ld\n", info->si_stime);
}

void nocldstop_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Entering SA_NOCLDSTOP child handler\n");
    printf("Signal number: %d, PID: %d, PPID: %d\n", sig, info->si_pid, getppid());
}

void resethand_handler(int sig) {
    printf("Entering SA_RESETHAND handler\n");
    printf("Signal number: %d, PID: %d, PPID: %d\n", sig, getpid(), getppid());
}


void send_signal_to_child(int sig) {
    pid_t pid;
    if ((pid = fork()) == 0) {
        while (1);
    } else {
        sleep(1);
        printf("%d sending %d to %d\n", getppid(), sig, pid);
        kill(pid, sig);
        sleep(1);
    }
}

void test_sa_siginfo() {
    printf("-----TESTING SA_SIGINFO-----\n");

    static struct sigaction sig;
    sig.sa_sigaction = sigint_handler;
    sigemptyset(&sig.sa_mask);
    if (sigaction(SIGINT, &sig, NULL) == -1)
        perror("Sigaction error\n");
    sig.sa_flags = 0;
    printf("SIGINFO not set yet\n");
    raise(SIGINT);

    printf("SIGINFO set\n");
    sig.sa_flags = SA_SIGINFO;
    if (sigaction(SIGINT, &sig, NULL) == -1)
        perror("Sigaction error\n");
    raise(SIGINT);
}

void test_sa_nocldstop() {
    printf("-----TESTING SA_NOCLDSTOP-----\n");
    static struct sigaction sig;
    sig.sa_sigaction = nocldstop_handler;
    sigemptyset(&sig.sa_mask);
    if (sigaction(SIGCHLD, &sig, NULL) == -1)
        perror("Sigaction error\n");

    printf("SA_NOCLDSTOP not set yet\n");
    send_signal_to_child(SIGSTOP);


    printf("SA_NOCLDSTOP set\n");
    sig.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sig, NULL) == -1)
        perror("Sigaction error\n");
    send_signal_to_child(SIGSTOP);
}

void test_sa_resethand() {
    printf("-----TESTING SA_RESETHAND-----\n");
    static struct sigaction sig;
    sig.sa_handler = resethand_handler;
    sigemptyset(&sig.sa_mask);
    if (sigaction(SIGCHLD, &sig, NULL) == -1)
        perror("Sigaction error\n");

    printf("SA_RESETHAND not set yet\n");
    send_signal_to_child(SIGSTOP);

    printf("SA_RESETHAND set\n");
    sig.sa_flags = SA_RESETHAND;
    send_signal_to_child(SIGSTOP);
}


int main(int argc, char *argv[]) {
    // SA_SIGINFO
    test_sa_siginfo();

    // SA_NOCLDSTOP
    test_sa_nocldstop();

    // SA_RESETHAND
    test_sa_resethand();
}