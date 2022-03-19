#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>
#include <string.h>
#include <ctype.h>

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

void lib_open_file(char *filename_1, char *filename_2) {
    char c;
    FILE *in_file, *out_file;
    if ((in_file = fopen(filename_1, "r")) == NULL || (out_file = fopen(filename_2, "w")) == NULL) {
        fprintf(stderr, "Cannot open a file");
        exit(1);
    }
    fpos_t index;
    int write_line;
    while (!feof(in_file)) {
        fgetpos(in_file, &index);
        write_line = 0;
        while (fread(&c, sizeof(char), 1, in_file) == 1) {
            if (c == '\n')
                break;
            else if (!isspace(c)) {
                write_line = 1;
                break;
            }
        }
        if (write_line == 1) {
            fsetpos(in_file, &index);
            while (fread(&c, sizeof(char), 1, in_file) == 1) {
                fwrite(&c, sizeof(char), 1, out_file);
                if (c == '\n')
                    break;
            }
        }
    }
    fclose(in_file);
    fclose(out_file);
}

void sys_open_file(char *filename_1, char *filename_2) {
    char c;
    int in_file, out_file;
    if ((in_file = open(filename_1, O_RDONLY)) < 0 || (out_file = open(filename_2, O_WRONLY)) < 0) {
        fprintf(stderr, "Cannot open a file");
        exit(1);
    }
    off_t index;
    int write_line;
    while (read(in_file, &c, 1) == 1) {
        lseek(in_file, -1, SEEK_CUR);
        write_line = 0;
        index = 0;
        while (read(in_file, &c, 1) == 1) {
            index++;
            if (c == '\n')
                break;
            else if (!isspace(c)) {
                write_line = 1;
                break;
            }
        }
        if (write_line == 1) {
            lseek(in_file, -index, SEEK_CUR);
            while (read(in_file, &c, 1) == 1) {
                write(out_file, &c, 1);
                if (c == '\n')
                    break;
            }
        }
    }
    close(in_file);
    close(out_file);
}

int main(int argc, char *argv[]) {
    char *filename_1, *filename_2;
    filename_1 = calloc(256, sizeof(char));
    filename_2 = calloc(256, sizeof(char));
    if (argc < 3) {
        fprintf(stdout, "Not enough arguments\n");

        fprintf(stdout, "Enter first filename: ");
        fscanf(stdin, "%s", filename_1);
        fprintf(stdout, "Enter second filename: ");
        fscanf(stdin, "%s", filename_2);

        filename_1 = realloc(filename_1, strlen(filename_1) + 1);
        filename_2 = realloc(filename_2, strlen(filename_2) + 1);
    } else if (argc == 3) {
        filename_1 = realloc(filename_1, strlen(argv[1]) + 1);
        filename_2 = realloc(filename_2, strlen(argv[2]) + 1);
        strcpy(filename_1, argv[1]);
        strcpy(filename_2, argv[2]);
    } else {
        fprintf(stdout, "Wrong number of arguments\n");
        exit(1);
    }
    FILE *results;
    results = fopen("pomiar_zad_1.txt", "w");

    // lib
    start_time = times(&start_time_buffer);
    lib_open_file(filename_1, filename_2);
    end_time = times(&end_time_buffer);
    print_result("lib", results);

    // sys
    start_time = times(&start_time_buffer);
    sys_open_file(filename_1, filename_2);
    end_time = times(&end_time_buffer);
    print_result("sys", results);
    fclose(results);

    free(filename_1);
    free(filename_2);
}