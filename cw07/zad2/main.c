#include "shared.h"

void SIGINT_handler(int);

void setup_shm_segment();

void setup_semaphores();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("Wrong number of arguments.\n");
        exit(1);
    }
    if (signal(SIGINT, SIGINT_handler) == SIG_ERR) {
        perror("ERROR! SIGINT signal error.\n");
        exit(1);
    }

    int cooks, suppliers;
    cooks = atoi(argv[1]);
    suppliers = atoi(argv[2]);
    if (cooks < 1 || suppliers < 1) {
        perror("ERROR! Number of cooks or suppliers cannot be lower than 1.\n");
        exit(1);
    }

    setup_shm_segment();
    setup_semaphores();

    for (int i = 0; i < cooks; i++) {
        if (fork() == 0) {
            execl("./cook", "./cook", NULL);
            perror("ERROR! execl() function error.\n");
        }
    }

    for (int i = 0; i < suppliers; i++) {
        if (fork() == 0) {
            execl("./supplier", "./supplier", NULL);
            perror("ERROR! execl() function error.\n");
        }
    }

    for (int i = 0; i < cooks + suppliers; i++) {
        wait(NULL);
    }
}

void SIGINT_handler(int sig_no) {
    printf("Received %d signal. Exiting bakery.\n", sig_no);
    sem_unlink(OVEN_SEMAPHORE);
    sem_unlink(TABLE_SEMAPHORE);
    sem_unlink(OVEN_FULL_SEMAPHORE);
    sem_unlink(TABLE_FULL_SEMAPHORE);
    sem_unlink(TABLE_EMPTY_SEMAPHORE);
    sem_unlink(OVEN_SHM);
    sem_unlink(TABLE_SHM);
}

void setup_shm_segment() {
    int oven_shm_fd, table_shm_fd;

    if ((oven_shm_fd = shm_open(OVEN_SHM, O_RDWR | O_CREAT, 0666)) == -1) {
        perror("ERROR! An error occurred while creating shared memory for oven.\n");
        exit(1);
    }
    if ((table_shm_fd = shm_open(TABLE_SHM, O_RDWR | O_CREAT, 0666)) == -1) {
        perror("ERROR! An error occurred while creating shared memory for table.\n");
        exit(1);
    }
    if (ftruncate(oven_shm_fd, sizeof(oven)) == -1) {
        perror("ERROR! ftruncate() error for oven.\n");
        exit(1);
    }
    if (ftruncate(table_shm_fd, sizeof(table)) == -1) {
        perror("ERROR! ftruncate() error for table.\n");
        exit(1);
    }

    // create new oven
    oven *new_oven;
    if ((new_oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, oven_shm_fd, 0)) == (void *) -1) {
        perror("ERROR! An error occurred while attaching shm segment for oven.\n");
        exit(1);
    }
    for (int i = 0; i < OVEN_SIZE; i++) {
        new_oven->ovens[i] = -1;
    }
    new_oven->pizzas = 0;
    new_oven->idx_to_place = 0;
    new_oven->idx_to_take_out = 0;

    // create mew table
    table *new_table;
    if ((new_table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0)) == (void *) -1) {
        perror("ERROR! An error occurred while attaching shm segment for table.\n");
        exit(1);
    }
    for (int i = 0; i < OVEN_SIZE; i++) {
        new_table->tables[i] = -1;
    }
    new_table->pizzas = 0;
    new_table->idx_to_place = 0;
    new_table->idx_to_take_out = 0;

    printf("Successfully created shared memory segment for oven with %d fd and table with %d fd.\n", oven_shm_fd,
           table_shm_fd);
}

void setup_semaphores() {
    sem_t *oven_semaphore, *table_semaphore;
    if ((oven_semaphore = sem_open(OVEN_SEMAPHORE, O_CREAT, 0666, 1)) == SEM_FAILED) {
        perror("ERROR! An error occurred while creating semaphore for oven.\n");
        exit(1);
    }
    if ((table_semaphore = sem_open(TABLE_SEMAPHORE, O_CREAT, 0666, 1)) == SEM_FAILED) {
        perror("ERROR! An error occurred while creating semaphore for table.\n");
        exit(1);
    }

    sem_t *oven_full_semaphore, *table_full_semaphore, *table_empty_semaphore;
    // if oven is full you cannot place any pizza, also block cook process
    if ((oven_full_semaphore = sem_open(OVEN_FULL_SEMAPHORE, O_CREAT, 0666, OVEN_SIZE)) == SEM_FAILED) {
        perror("ERROR! An error occurred while creating full oven semaphore.\n");
        exit(1);
    }

    // if table is full you cannot place any pizza, also block cook process
    if ((table_full_semaphore = sem_open(TABLE_FULL_SEMAPHORE, O_CREAT, 0666, TABLE_SIZE)) == SEM_FAILED) {
        perror("ERROR! An error occurred while creating full table semaphore.\n");
        exit(1);
    }

    // if table is empty you cannot take out any pizza, also block cook process
    if ((table_empty_semaphore = sem_open(TABLE_EMPTY_SEMAPHORE, O_CREAT, 0666, 0)) == SEM_FAILED) {
        perror("ERROR! An error occurred while creating empty table semaphore.\n");
        exit(1);
    }

    printf("Successfully created semaphores.\n");
}