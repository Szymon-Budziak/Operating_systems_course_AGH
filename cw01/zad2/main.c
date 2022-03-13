#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include "../zad1/lib.c"
#include <dlfcn.h>

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

static clock_t start_time, end_time;
static struct tms start_time_buffer, end_time_buffer;
static void *lib_handle;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / (double) sysconf(_SC_CLK_TCK);
}

void load_dynamic_library() {
    lib_handle = dlopen("lib.so", RTLD_LAZY);
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
}

int main(int argc, char *argv[]) {
#ifdef DYNAMIC
    load_dynamic_library();
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
            if (i + 1 >= argc) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            size = atoi(argv[i + 1]);
            create_table(size);
        } else if (strcmp(argv[i], "wc_files") == 0) {
            arg_length = 0;
            int j = i + 1;
            for (; j < argc; j++) {
                if (strcmp(argv[j], "create_table") != 0 || strcmp(argv[j], "wc_files") != 0 ||
                    strcmp(argv[j], "remove_block") != 0)
                    arg_length++;
                else
                    break;
            }
            if (arg_length == 0) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            wc_files(argv + i + 1, arg_length);
            index = save_in_memory();
            i = j - 1;
        } else if (strcmp(argv[i], "remove_block") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Wrong number of arguments");
                exit(1);
            }
            index = atoi(argv[i + 1]);
            remove_block(index);
        }
    }
    remove_array();
    end_time = times(&end_time_buffer);
    printf("[REAL TIME] main.c execution took %fs\n"
           "[USER TIME] main.c execution took %fs\n"
           "[SYSTEM TIME] main.c execution took %fs\n",
           calculate_time(end_time, start_time),
           calculate_time(start_time_buffer.tms_utime, end_time_buffer.tms_utime),
           calculate_time(start_time_buffer.tms_stime, end_time_buffer.tms_stime));
#ifdef DYNAMIC
    void dlclose(lib_handle);
#endif
    return 0;
}