#ifndef LAB1_LIB
#define LAB1_LIB

typedef struct memory_block {
    unsigned int size;
    char **result;
} memory_block;

typedef struct table {
    int size;
    memory_block **blocks;
} wrapped_array;

void create_table(int size);

void wc_files(char *files[], int length);

int save_in_memory();

void remove_array();

void remove_block(int index);

#endif