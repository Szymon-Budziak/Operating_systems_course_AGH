#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <string.h>

clock_t start_time, end_time;
struct tms start_time_buffer, end_time_buffer;;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / (double) sysconf(_SC_CLK_TCK);
}

void print_result(FILE *file) {
    printf("[REAL TIME] execution took %fs\n"
           "[USER TIME] execution took %fs\n"
           "[SYSTEM TIME] execution took %fs\n",
           calculate_time(start_time, end_time),
           calculate_time(start_time_buffer.tms_utime, end_time_buffer.tms_utime),
           calculate_time(start_time_buffer.tms_stime, end_time_buffer.tms_stime));
    fprintf(file, "[REAL TIME] execution took %fs\n"
                  "[USER TIME] execution took %fs\n"
                  "[SYSTEM TIME] execution took %fs\n",
            calculate_time(start_time, end_time),
            calculate_time(start_time_buffer.tms_utime, end_time_buffer.tms_utime),
            calculate_time(start_time_buffer.tms_stime, end_time_buffer.tms_stime));
}

double function(double x) {
    return 4 / (x * x + 1);
}

double calculate_partial_integral(double start, double end, double width) {
    double result;
    result = 0;
    while (start < end) {
        result += function(start) * width;
        start += width;
    }
    return result;
}

double calculate_integral(double width, int n) {
    FILE *in_file;
    char *in_filename;
    double start, end, partial_integral, step;

    step = 1 / (double) n;
    start = 0;
    end = 1 / (double) n;
    in_filename = calloc(20, sizeof(char));
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            partial_integral = calculate_partial_integral(start, end, width);
            sprintf(in_filename, "wn_files/w%d.txt", i + 1);
            in_file = fopen(in_filename, "w");
            fprintf(in_file, "%lf\n", partial_integral);
            fclose(in_file);
            exit(0);
        }
        start += step;
        end += step;
    }
    free(in_filename);

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    double result;
    char *out_filename, *buffer, *p;
    size_t file_size;
    FILE *out_file;

    result = 0;
    out_filename = calloc(20, sizeof(char));
    for (int i = 0; i < n; i++) {
        sprintf(out_filename, "wn_files/w%d.txt", i + 1);
        out_file = fopen(out_filename, "r");

        fseek(out_file, 0, SEEK_END);
        file_size = ftell(out_file);
        fseek(out_file, 0, SEEK_SET);

        buffer = calloc(file_size + 1, sizeof(char));
        fread(buffer, sizeof(char), file_size + 1, out_file);

        result += strtod(buffer, &p);
        fclose(out_file);
        free(buffer);
    }
    free(out_filename);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    double width, result;
    int n;
    char *p;
    width = strtod(argv[1], &p);
    if (*p != '\0') {
        fprintf(stderr, "Invalid parameter, which is not a number\n");
        exit(1);
    }
    n = atoi(argv[2]);

    if (n > 1 / width) {
        fprintf(stderr, "Not enough rectangles for processes\n");
        exit(1);
    }
    start_time = times(&start_time_buffer);
    result = calculate_integral(width, n);
    end_time = times(&end_time_buffer);

    FILE *output_file;
    output_file = fopen("report.txt", "w");
    print_result(output_file);
    fprintf(stdout, "Integral result: %f\n", result);
    fprintf(output_file, "Integral result: %f\n", result);
    fclose(output_file);
}