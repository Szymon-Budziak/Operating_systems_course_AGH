#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

static wrapped_array *array;
static char command[100000];

void create_table(int size) {
    if (size <= 0) {
        fprintf(stderr, "Wrong size");
        return;
    }
    array = calloc(1, sizeof(wrapped_array));
    array->size = size;
    array->blocks = calloc(size, sizeof(memory_block));
}

void wc_files(char *files[], int length) {
    static const char *wc = "wc";
    static const char *tmp_file = " > tmp_file.txt";
    static int system_command;
    strcat(command, wc);
    for (int i = 0; i < length; i++) {
        strcat(command, " ");
        strcat(command, files[i]);
    }
    strcat(command, tmp_file);
    system_command = system(command);
    if (system_command == -1)
        fprintf(stderr, "Wrong command");
}

int save_in_memory() {
    FILE *tmp_file;
    static unsigned int file_size;
    static int index, remove_tmp_file;
    static unsigned long fread_run;

    /* get size of tmp_file in which are saved wc results */
    tmp_file = fopen("tmp_file.txt", "r");
    fseek(tmp_file, 0, SEEK_END);
    file_size = ftell(tmp_file);
    fseek(tmp_file, 0, SEEK_SET);

    /* searching for empty block */
    index = 0;
    while (array->blocks[index] != NULL && index < array->size) {
        index++;
    }
    /* allocating memory */
    array->blocks[index] = calloc(1, sizeof(memory_block));
    array->blocks[index]->size = file_size;
    array->blocks[index]->result = calloc(file_size + 1, sizeof(char));
    fread_run = fread(array->blocks[index]->result, sizeof(char), file_size, tmp_file);
    if (fread_run == -1)
        fprintf(stderr, "Fread failed");
    fclose(tmp_file);
    remove_tmp_file = system("rm -rf tmp_file.txt");
    if (remove_tmp_file == -1)
        fprintf(stderr, "Wrong file");
    return index;
}

void remove_array() {
    if (array == NULL || array->blocks == NULL) {
        fprintf(stderr, "Array is already free");
        return;
    }
    int i;
    for (i = 0; i < array->size; i++) {
        if (array->blocks[i] != NULL) {
            free(array->blocks[i]->result);
            array->blocks[i]->result = NULL;
            free(array->blocks[i]);
            array->blocks[i] = NULL;
        }
    }
    free(array);
}


void remove_block(int index) {
    if (index < 0 || index >= array->size || array->blocks[index] == NULL) {
        fprintf(stderr, "Wrong index.");
        return;
    }
    free(array->blocks[index]->result);
    array->blocks[index]->result = NULL;
    free(array->blocks[index]);
    array->blocks[index] = NULL;
}