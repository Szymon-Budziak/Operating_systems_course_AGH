#ifndef SHARED_H
#define SHARED_H

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define OVEN_SIZE 5
#define OVEN_SEMAPHORE "/OVEN_SEM"
#define OVEN_FULL_SEMAPHORE "/OVEN_FULL_SEM"
#define OVEN_SHM "/oven_shm"

#define TABLE_SIZE 5
#define TABLE_SEMAPHORE "/TABLE_SEM"
#define TABLE_FULL_SEMAPHORE "/TABLE_FULL_SEM"
#define TABLE_EMPTY_SEMAPHORE "/TABLE_EMPTY_SEM"
#define TABLE_SHM "/table_shm"

#define SLEEP_TIME 1
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

typedef struct oven {
    int ovens[OVEN_SIZE];
    int pizzas;
    int idx_to_place;
    int idx_to_take_out;
} oven;

typedef struct table {
    int tables[TABLE_SIZE];
    int pizzas;
    int idx_to_place;
    int idx_to_take_out;
} table;

sem_t *get_semaphore(char *name) {
    sem_t *semaphore;
    if ((semaphore = sem_open(name, O_RDWR)) == SEM_FAILED) {
        perror("ERROR! An error occurred while connecting to semaphore.\n");
        exit(1);
    }
    return semaphore;
}

int get_oven_shm_fd() {
    int oven_shm_fd;
    if ((oven_shm_fd = shm_open(OVEN_SHM, O_RDWR, 0666)) == -1) {
        perror("ERROR! An error occurred while getting oven shm fd.\n");
        exit(1);
    }
    return oven_shm_fd;
}

int get_table_shm_fd() {
    int table_shm_fd;
    if ((table_shm_fd = shm_open(TABLE_SHM, O_RDWR, 0666)) == -1) {
        perror("ERROR! An error occurred while getting table shm fd.\n");
        exit(1);
    }
    return table_shm_fd;
}

void lock_semaphore(sem_t *semaphore) {
    if (sem_wait(semaphore) == -1) {
        perror("ERROR! An error occurred while locking semaphore.\n");
        exit(1);
    }
}

void unlock_semaphore(sem_t *semaphore) {
    if (sem_post(semaphore) == -1) {
        perror("ERROR! An error occurred while unlocking semaphore.\n");
        exit(1);
    }
}

char *get_time() {
    struct timeval time;
    char buffer[100];
    char *curr_time;
    curr_time = calloc(100, sizeof(char));
    gettimeofday(&time, NULL);

    strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", localtime(&time.tv_sec));
    sprintf(curr_time, "%s:%03ld", buffer, time.tv_usec / 1000);
    return curr_time;
}

#endif // SHARED_H