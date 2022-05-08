#include "shared.h"

void put_in_oven(oven *, int);

void place_on_table(table *, int);

int deliver_pizza(oven *);

int main(int argc, char *argv[]) {
    int oven_shm_id, table_shm_id, semaphore_id;
    int type;

    semaphore_id = get_semaphore_id();
    oven_shm_id = get_oven_shm_id();
    table_shm_id = get_table_shm_id();

    oven *new_oven;
    new_oven = shmat(oven_shm_id, NULL, 0);
    table *new_table;
    new_table = shmat(table_shm_id, NULL, 0);

    srand(getpid());
    while (true) {
        type = rand() % 10;
        printf("(pid: %d, timestamp: %s)\t Przygotowuje pizze: %d\n", getpid(), get_time(), type);
        sleep(SLEEP_TIME);

        // if OVEN_FULL_SEMAPHORE is 0 then blocks cook process
        lock_semaphore(semaphore_id, OVEN_FULL_SEMAPHORE);

        lock_semaphore(semaphore_id, OVEN_SEMAPHORE);
        put_in_oven(new_oven, type);
        printf("(pid: %d, timestamp: %s)\t Dodano pizee: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time(), type,
               new_oven->pizzas);
        unlock_semaphore(semaphore_id, OVEN_SEMAPHORE);

        sleep(BAKING_TIME);

        lock_semaphore(semaphore_id, OVEN_SEMAPHORE);
        type = deliver_pizza(new_oven);
        printf("(pid: %d, timestamp: %s)\t Wyjęto pizee: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), type, new_oven->pizzas, new_table->pizzas);
        unlock_semaphore(semaphore_id, OVEN_SEMAPHORE);

        unlock_semaphore(semaphore_id, OVEN_FULL_SEMAPHORE);

        // if TABLE_FULL_SEMAPHORE is 0 then blocks cook process
        lock_semaphore(semaphore_id, TABLE_FULL_SEMAPHORE);

        lock_semaphore(semaphore_id, TABLE_SEMAPHORE);
        place_on_table(new_table, type);
        printf("(pid: %d, timestamp: %s)\t Pizza umieszczona na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
               getpid(), get_time(), type, new_oven->pizzas, new_table->pizzas);
        unlock_semaphore(semaphore_id, TABLE_SEMAPHORE);

        // if TABLE_EMPTY_SEMAPHORE is 0 then blocks supplier process
        unlock_semaphore(semaphore_id, TABLE_EMPTY_SEMAPHORE);
    }
}

void put_in_oven(oven *ove, int type) {
    ove->ovens[ove->idx_to_place] = type;
    ove->idx_to_place = (ove->idx_to_place + 1) % OVEN_SIZE;
    ove->pizzas++;
}

void place_on_table(table *tab, int type) {
    tab->tables[tab->idx_to_place] = type;
    tab->idx_to_place = (tab->idx_to_place + 1) % OVEN_SIZE;
    tab->pizzas++;
}

int deliver_pizza(oven *ove) {
    int type;
    type = ove->ovens[ove->idx_to_take_out];
    ove->ovens[ove->idx_to_take_out] = -1;
    ove->idx_to_take_out = (ove->idx_to_take_out + 1) % OVEN_SIZE;
    ove->pizzas--;
    return type;
}