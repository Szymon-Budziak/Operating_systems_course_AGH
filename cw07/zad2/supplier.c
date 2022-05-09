#include "shared.h"

int deliver_pizza(table *);

int main(int argc, char *argv[]) {
    sem_t *table_semaphore, *table_full_semaphore, *table_empty_semaphore;
    int type;
    int table_shm_fd;

    table_semaphore = get_semaphore(TABLE_SEMAPHORE);
    table_full_semaphore = get_semaphore(TABLE_FULL_SEMAPHORE);
    table_empty_semaphore = get_semaphore(TABLE_EMPTY_SEMAPHORE);

    table_shm_fd = get_table_shm_fd();

    table *new_table;
    new_table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0);

    while (true) {
        // if TABLE_EMPTY_SEMAPHORE is 0 then blocks supplier process
        lock_semaphore(table_empty_semaphore);

        lock_semaphore(table_semaphore);
        type = deliver_pizza(new_table);
        printf("(pid: %d, timestamp: %s)\t Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_time(), type,
               new_table->pizzas);
        unlock_semaphore(table_semaphore);

        // if TABLE_FULL_SEMAPHORE is 0 then blocks cook process
        unlock_semaphore(table_full_semaphore);

        sleep(DELIVERY_TIME);
        printf("(pid: %d, timestamp: %s)\t Dostarczono pizze: %d.\n", getpid(), get_time(), type);
        sleep(RETURN_TIME);
    }
}

int deliver_pizza(table *new_table) {
    int type;
    type = new_table->tables[new_table->idx_to_take_out];
    new_table->tables[new_table->idx_to_take_out] = -1;
    new_table->idx_to_take_out = (new_table->idx_to_take_out + 1) % TABLE_SIZE;
    new_table->pizzas--;
    return type;
}