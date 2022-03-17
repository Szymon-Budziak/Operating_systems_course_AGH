#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include "../zad1/lib.h"

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

static clock_t start_time, end_time;
static struct tms start_time_buffer, end_time_buffer;
static clock_t additional_start_time, additional_end_time;
static struct tms additional_start_time_buffer, additional_end_time_buffer;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / (double) sysconf(_SC_CLK_TCK);
}

void print_result(clock_t s_time, clock_t e_time, struct tms s_buffer, struct tms e_buffer, char name[]) {
    printf("[REAL TIME] %s execution took %fs\n"
           "[USER TIME] %s execution took %fs\n"
           "[SYSTEM TIME] %s execution took %fs\n",
           name, calculate_time(s_time, e_time),
           name, calculate_time(s_buffer.tms_utime, e_buffer.tms_utime),
           name, calculate_time(s_buffer.tms_stime, e_buffer.tms_stime));
}

int main(int argc, char *argv[]) {
#ifdef DYNAMIC
    void *lib_handle = dlopen("../zad1/lib.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Cannot load a file");
        exit(1);
    }
    void (*create_table)(int);
    void (*wc_files)(char **, int);
    int (*save_in_memory)(void);
    void (*remove_array)(void);
    void (*remove_block)(int);
    create_table = (void (*)(int)) dlsym(lib_handle, "create_table");
    wc_files = (void (*)(char **, int)) dlsym(lib_handle, "wc_files");
    save_in_memory = (int (*)(void)) dlsym(lib_handle, "save_in_memory");
    remove_array = (void (*)(void)) dlsym(lib_handle, "remove_array");
    remove_block = (void (*)(int)) dlsym(lib_handle, "remove_block");
    if (dlerror() != NULL) {
        fprintf(stderr, "Cannot load lib file");
        exit(1);
    }
#endif

    int i, size, arg_length, index;

    if (argc <= 1) {
        fprintf(stderr, "Not enough arguments");
        exit(1);
    }
    i = 0;
    while (i < argc) {
        start_time = times(&start_time_buffer);
        if (strcmp(argv[i], "create_table") == 0) {
            additional_start_time = times(&additional_start_time_buffer);
            if (i + 1 >= argc) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            size = atoi(argv[i + 1]);
            create_table(size);
            additional_end_time = times(&additional_end_time_buffer);
            print_result(additional_start_time, additional_end_time, additional_start_time_buffer,
                         additional_end_time_buffer, "create_table");
        } else if (strcmp(argv[i], "wc_files") == 0) {
            additional_start_time = times(&additional_start_time_buffer);
            arg_length = 0;
            int j = i + 1;
            for (; j < argc; j++) {
                if (strcmp(argv[j], "create_table") == 0 || strcmp(argv[j], "wc_files") == 0 ||
                    strcmp(argv[j], "remove_block") == 0)
                    break;
                else
                    arg_length++;
            }
            if (arg_length == 0) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            wc_files(argv + i + 1, arg_length);
            index = save_in_memory();
            i = j - 1;
            additional_end_time = times(&additional_end_time_buffer);
            print_result(additional_start_time, additional_end_time, additional_start_time_buffer,
                         additional_end_time_buffer, "wc_files");
        } else if (strcmp(argv[i], "remove_block") == 0) {
            additional_start_time = times(&additional_start_time_buffer);
            if (i + 1 >= argc) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            index = atoi(argv[i + 1]);
            remove_block(index);
            additional_end_time = times(&additional_end_time_buffer);
            print_result(additional_start_time, additional_end_time, additional_start_time_buffer,
                         additional_end_time_buffer, "remove_block");
        }
        i++;
        for (int j = 0; j < 100000000; ++j) { ;
        }
    }
    remove_array();
    end_time = times(&end_time_buffer);
    print_result(start_time, end_time, start_time_buffer, end_time_buffer, "total_time");
#ifdef DYNAMIC
    dlclose(lib_handle);
#endif
    return 0;
}