#include "shared.h"

void put_in_oven(oven *, int);

void place_on_table(table *, int);

int deliver_pizza(oven *);

int main(int argc, char *argv[]) {
    sem_t *oven_semaphore, *table_semaphore;
    sem_t *oven_full_semaphore, *table_full_semaphore, *table_empty_semaphore;
    int type;
    int oven_shm_fd, table_shm_fd;

    oven_semaphore = get_semaphore(OVEN_SEMAPHORE);
    table_semaphore = get_semaphore(TABLE_SEMAPHORE);
    oven_full_semaphore = get_semaphore(OVEN_FULL_SEMAPHORE);
    table_full_semaphore = get_semaphore(TABLE_FULL_SEMAPHORE);
    table_empty_semaphore = get_semaphore(TABLE_EMPTY_SEMAPHORE);

    oven_shm_fd = get_oven_shm_fd();
    table_shm_fd = get_table_shm_fd();

    oven *new_oven;
    new_oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, oven_shm_fd, 0);
    table *new_table;
    new_table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0);

    srand(getpid());
    while (true) {
        type = rand() % 10;
        printf("(pid: %d, timestamp: %s)\t Przygotowuje pizze: %d\n", getpid(), get_time(), type);
        sleep(SLEEP_TIME);

        // if OVEN_FULL_SEMAPHORE is 0 then blocks cook process
        lock_semaphore(oven_full_semaphore);

        lock_semaphore(oven_semaphore);
        put_in_oven(new_oven, type);
        printf("(pid: %d, timestamp: %s)\t Dodano pizee: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time(), type,
               new_oven->pizzas);
        unlock_semaphore(oven_semaphore);

        sleep(BAKING_TIME);

        lock_semaphore(oven_semaphore);
        type = deliver_pizza(new_oven);
        printf("(pid: %d, timestamp: %s)\t Wyjęto pizee: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), type, new_oven->pizzas, new_table->pizzas);
        unlock_semaphore(oven_semaphore);

        unlock_semaphore(oven_full_semaphore);

        // if TABLE_FULL_SEMAPHORE is 0 then blocks cook process
        lock_semaphore(table_full_semaphore);

        lock_semaphore(table_semaphore);
        place_on_table(new_table, type);
        printf("(pid: %d, timestamp: %s)\t Pizza umieszczona na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), type, new_oven->pizzas, new_table->pizzas);
        unlock_semaphore(table_semaphore);

        // if TABLE_EMPTY_SEMAPHORE is 0 then blocks supplier process
        unlock_semaphore(table_empty_semaphore);
    }
}

void put_in_oven(oven *new_oven, int type) {
    new_oven->ovens[new_oven->idx_to_place] = type;
    new_oven->idx_to_place = (new_oven->idx_to_place + 1) % OVEN_SIZE;
    new_oven->pizzas++;
}

void place_on_table(table *new_table, int type) {
    new_table->tables[new_table->idx_to_place] = type;
    new_table->idx_to_place = (new_table->idx_to_place + 1) % OVEN_SIZE;
    new_table->pizzas++;
}

int deliver_pizza(oven *new_oven) {
    int type;
    type = new_oven->ovens[new_oven->idx_to_take_out];
    new_oven->ovens[new_oven->idx_to_take_out] = -1;
    new_oven->idx_to_take_out = (new_oven->idx_to_take_out + 1) % OVEN_SIZE;
    new_oven->pizzas--;
    return type;
}