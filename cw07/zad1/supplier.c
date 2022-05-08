#include "shared.h"

int deliver_pizza(table *);

int main(int argc, char *argv[]) {
    int table_shm_id, semaphore_id;
    int type;

    semaphore_id = get_semaphore_id();
    table_shm_id = get_table_shm_id();

    table *new_table;
    new_table = shmat(table_shm_id, NULL, 0);

    while (true) {
        // if TABLE_EMPTY_SEMAPHORE is 0 then blocks supplier process
        lock_semaphore(semaphore_id, TABLE_EMPTY_SEMAPHORE);

        lock_semaphore(semaphore_id, TABLE_SEMAPHORE);
        type = deliver_pizza(new_table);
        printf("(pid: %d, timestamp: %s)\t Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_time(), type,
               new_table->pizzas);
        unlock_semaphore(semaphore_id, TABLE_SEMAPHORE);

        // if TABLE_FULL_SEMAPHORE is 0 then blocks cook process
        unlock_semaphore(semaphore_id, TABLE_FULL_SEMAPHORE);

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