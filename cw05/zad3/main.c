#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

void create_file(char *filename, int rows_number) {
    FILE *file;
    char *line;
    file = fopen(filename, "w");
    line = calloc(256, sizeof(char));
    // creating empty line
    for (int i = 0; i < 256; i++) {
        line[i] = ' ';
        if (i == 255)
            line[255] = '\n';
    }
    // adding empty lines to a file
    for (int i = 0; i < rows_number; i++) {
        if (i == rows_number - 1)
            line[255] = '\0';
        fprintf(file, "%s", line);
    }
    fclose(file);
    free(line);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("Wrong number of arguments\n");
        exit(1);
    }

    int producers_number, consumers_number;
    producers_number = atoi(argv[1]);
    if (producers_number < 1 || producers_number > 5) {
        perror("Wrong number of producers\n");
        exit(1);
    }

    consumers_number = atoi(argv[2]);
    if (consumers_number < 1 || consumers_number > 5) {
        perror("Wrong number of consumers\n");
        exit(1);
    }
    mkfifo("fifo", 0666);

    char *result_file = "consumer_files/consumer_result.txt";
    for (int i = 0; i < consumers_number; i++) {
        if (fork() == 0) {
            if (execl("./consumer", "./consumer", "fifo", result_file, "10", NULL) == -1) {
                perror("execl error\n");
                exit(1);
            }
        }
    }

    create_file(result_file, producers_number);

    char row[3], input_file[100];
    int exec_result;
    for (int i = 0; i < producers_number; i++) {
        sprintf(row, "%d", i);
        if (fork() == 0) {
            sprintf(input_file, "producer_files/producer_file_%d.txt", i + 1);
            if ((exec_result = execl("./producer", "./producer", "fifo", row, input_file, "5", NULL)) != -1) {
                printf("%d", exec_result);
            }
        }
    }
    for (int i = 0; i < consumers_number + producers_number; i++) {
        wait(NULL);
    }
}