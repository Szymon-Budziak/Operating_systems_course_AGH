#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

void write_to_file(FILE *file, char *msg, int row_number) {
    rewind(file);
    int row_count, i;
    char buffer[257];

    row_count = 0;
    printf("[CONSUMER] Saving line to result file in %d row\n", row_number);
    while (fgets(buffer, 257, file) != NULL) {
        if (row_count == row_number) {
            for (i = 0; i < 256; i++) {
                if (buffer[i] == ' ')
                    break;
            }
            flock(fileno(file), LOCK_EX);
            fseek(file, row_number * 256 + i, SEEK_SET);
            fprintf(file, msg, strlen(msg) + 1);
            flock(fileno(file), LOCK_UN);
            printf("[CONSUMER] Wrote message: %s\n\n", msg);
            fflush(file);
            break;
        }
        row_count++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    FILE *fifo, *file;
    int N;

    if ((fifo = fopen(argv[1], "r")) == NULL) {
        perror("Cannot open a stream\n");
        exit(1);
    }

    if ((file = fopen(argv[2], "r+")) == NULL) {
        perror("Cannot open a file\n");
        exit(1);
    }
    N = atoi(argv[3]);

    char buffer[N], line[257];
    char *sequence;
    int number;
    while (fread(buffer, sizeof(char), N, fifo) == N) {
        printf("[CONSUMER] Read from fifo: %s", buffer);
        sequence = strtok(buffer, "#");
        number = atoi(sequence);
        sequence = strtok(NULL, "\n");
        sprintf(line, "%s", sequence);
        write_to_file(file, line, number);
    }
    fclose(fifo);
    fclose(file);
}