#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/file.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    FILE *fifo, *file;
    int row_number, N;

    if ((fifo = fopen(argv[1], "w")) == NULL) {
        perror("Cannot open a stream\n");
        exit(1);
    }
    row_number = atoi(argv[2]);

    if ((file = fopen(argv[3], "r")) == NULL) {
        perror("Cannot open a file\n");
        exit(1);
    }
    N = atoi(argv[4]);

    char buffer[N];
    char msg[N + 5];
    srand(time(NULL));
    while (fread(buffer, sizeof(char), N, file) == N) {
        sleep(rand() % 2 + 1);
        buffer[strlen(buffer) - 1] = '\n';
        sprintf(msg, "%d#%s", row_number, buffer);
        flock(fileno(fifo), LOCK_EX);
        fwrite(msg, sizeof(char), N + 5, fifo);
        flock(fileno(fifo), LOCK_UN);
        printf("[PRODUCER] Wrote to fifo: %s\n", buffer);
    }
    fclose(fifo);
    fclose(file);
}