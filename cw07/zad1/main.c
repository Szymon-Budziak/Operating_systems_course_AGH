#include "shared.h"

int oven_shm_id, table_shm_id, semaphore_id;

void SIGINT_handler(int);

void setup_shm_segment();

void setup_semaphore();

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
    setup_semaphore();

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
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    shmctl(table_shm_id, IPC_RMID, NULL);
}

void setup_shm_segment() {
    key_t oven_key, table_key;

    if ((oven_key = ftok(OVEN_PATH, OVEN_ID)) == -1) {
        perror("ERROR! An error occurred while creating oven key.\n");
        exit(1);
    }
    if ((table_key = ftok(TABLE_PATH, TABLE_ID)) == -1) {
        perror("ERROR! An error occurred while creating table key.\n");
        exit(1);
    }
    printf("Oven created with %d key and table created with %d key.\n", oven_key, table_key);

    if ((oven_shm_id = shmget(oven_key, sizeof(oven), IPC_CREAT | 0666)) == -1) {
        perror("ERROR! An error occurred while creating shared memory for oven.\n");
        exit(1);
    }
    if ((table_shm_id = shmget(table_key, sizeof(table), IPC_CREAT | 0666)) == -1) {
        perror("ERROR! An error occurred while creating shared memory for table.\n");
        exit(1);
    }

    // create new oven
    oven *new_oven;
    new_oven = shmat(oven_shm_id, NULL, 0);
    for (int i = 0; i < OVEN_SIZE; i++) {
        new_oven->ovens[i] = -1;
    }
    new_oven->pizzas = 0;
    new_oven->idx_to_place = 0;
    new_oven->idx_to_take_out = 0;

    // create new table
    table *new_table;
    new_table = shmat(table_shm_id, NULL, 0);
    for (int i = 0; i < OVEN_SIZE; i++) {
        new_table->tables[i] = -1;
    }
    new_table->pizzas = 0;
    new_table->idx_to_place = 0;
    new_table->idx_to_take_out = 0;

    printf("Successfully created shared memory for oven with %d id and table with %d id.\n", oven_shm_id, table_shm_id);
}

void setup_semaphore() {
    char *home_path;
    key_t key;

    if ((home_path = getenv("HOME")) == NULL) {
        perror("ERROR! An error occurred while setting home path.\n");
        exit(1);
    }
    if ((key = ftok(home_path, PROJECT_ID)) == -1) {
        perror("ERROR! An error occurred while creating key.\n");
        exit(1);
    }
    if ((semaphore_id = semget(key, 5, 0666 | IPC_CREAT)) == -1) {
        perror("ERROR! An error occurred with semaphore id.\n");
        exit(1);
    }

    union semun;
    semun.val = 1;

    if (semctl(semaphore_id, OVEN_SEMAPHORE, SETVAL, semun) == -1) {
        perror("ERROR! An error occurred while setting oven semaphore value.\n");
        exit(1);
    }
    if (semctl(semaphore_id, TABLE_SEMAPHORE, SETVAL, semun) == -1) {
        perror("ERROR! An error occurred while setting table semaphore value.\n");
        exit(1);
    }

    // if oven is full you cannot place any pizza, also block cook process
    semun.val = OVEN_SIZE;
    if (semctl(semaphore_id, OVEN_FULL_SEMAPHORE, SETVAL, semun) == -1) {
        perror("ERROR! An error occurred while setting full oven semaphore value.\n");
        exit(1);
    }

    // if table is full you cannot place any pizza, also block cook process
    semun.val = TABLE_SIZE;
    if ((semctl(semaphore_id, TABLE_FULL_SEMAPHORE, SETVAL, semun)) == -1) {
        perror("ERROR! An error occurred while setting full table semaphore value.\n");
        exit(1);
    }

    // if table is empty you cannot take out any pizza, also block cook process
    semun.val = 0;
    if ((semctl(semaphore_id, TABLE_EMPTY_SEMAPHORE, SETVAL, semun)) == -1) {
        perror("ERROR! An error occurred while setting empty table semaphore value.\n");
        exit(1);
    }

    printf("Successfully created semaphore set with %d id.\n", semaphore_id);
}