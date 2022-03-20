#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <fcntl.h>

static clock_t start_time, end_time;
static struct tms start_time_buffer, end_time_buffer;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / (double) sysconf(_SC_CLK_TCK);
}

void print_result(char name[], FILE *file) {
    printf("[REAL TIME] %s execution took %fs\n"
           "[USER TIME] %s execution took %fs\n"
           "[SYSTEM TIME] %s execution took %fs\n",
           name, calculate_time(start_time, end_time),
           name, calculate_time(start_time_buffer.tms_utime, end_time_buffer.tms_utime),
           name, calculate_time(start_time_buffer.tms_stime, end_time_buffer.tms_stime));
    fprintf(file, "[REAL TIME] %s execution took %fs\n"
                  "[USER TIME] %s execution took %fs\n"
                  "[SYSTEM TIME] %s execution took %fs\n",
            name, calculate_time(start_time, end_time),
            name, calculate_time(start_time_buffer.tms_utime, end_time_buffer.tms_utime),
            name, calculate_time(start_time_buffer.tms_stime, end_time_buffer.tms_stime));
}

void lib_find_character(char *filename, char character) {
    char c;
    int count, rows, found;
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Cannot open a file");
        exit(1);
    }
    count = 0;
    rows = 0;
    found = 0;
    while (fread(&c, sizeof(char), 1, file) == 1) {
        if (c == '\n') {
            found = 0;
        }
        if (character == c) {
            count++;
            if (found == 0) {
                rows++;
                found = 1;
            }
        }
    }
    fprintf(stdout, "Number of occurrences %c is %d and rows %d\n", character, count, rows);
    fclose(file);
}

void sys_find_character(char *filename, char character) {
    char c;
    int count, rows, found;
    int in_file;
    if ((in_file = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Cannot open a file");
        exit(1);
    }
    count = 0;
    rows = 0;
    found = 0;
    while (read(in_file, &c, 1) == 1) {
        if (c == '\n') {
            found = 0;
        }
        if (character == c) {
            count++;
            if (found == 0) {
                rows++;
                found = 1;
            }
        }
    }
    fprintf(stdout, "Number of occurrences %c is %d and rows %d\n", character, count, rows);
    close(in_file);
}

int main(int argc, char *argv[]) {
    char character;
    char *filename;
    filename = calloc(256, sizeof(char));
    if (argc < 3) {
        fprintf(stdout, "Not enough arguments\n");
        exit(1);
    } else if (argc == 3) {
        character = *(argv[1]);
        filename = realloc(filename, strlen(filename) + 1);
        strcpy(filename, argv[2]);
    } else {
        fprintf(stdout, "Wrong number of arguments\n");
        exit(1);
    }
    FILE *results;
    results = fopen("pomiar_zad_2.txt", "w");

    // lib
    start_time = times(&start_time_buffer);
    lib_find_character(filename, character);
    end_time = times(&end_time_buffer);
    print_result("lib", results);

    // sys
    start_time = times(&start_time_buffer);
    sys_find_character(filename, character);
    end_time = times(&end_time_buffer);
    print_result("sys", results);

    fclose(results);
    free(filename);
}