#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <signal.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#define OVEN_ID 'O'
#define OVEN_PATH "oven"
#define OVEN_SIZE 5
#define OVEN_SEMAPHORE 0
#define OVEN_FULL_SEMAPHORE 2

#define TABLE_ID 'T'
#define TABLE_PATH "table"
#define TABLE_SIZE 5
#define TABLE_SEMAPHORE 1
#define TABLE_FULL_SEMAPHORE 3
#define TABLE_EMPTY_SEMAPHORE 4

#define PROJECT_ID 'I'
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

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} semun;

int get_semaphore_id() {
    key_t key;
    char *home_path;
    int semaphore_id;
    if ((home_path = getenv("HOME")) == NULL) {
        perror("ERROR! An error occurred while setting home path.\n");
        exit(1);
    }
    if ((key = ftok(home_path, PROJECT_ID)) == -1) {
        perror("ERROR! An error occurred while creating key.\n");
        exit(1);
    }
    if ((semaphore_id = semget(key, 5, 0)) == -1) {
        perror("ERROR! An error occurred with semaphore id.\n");
        exit(1);
    }
    return semaphore_id;
}

int get_oven_shm_id() {
    key_t oven_key;
    int oven_shm_id;
    if ((oven_key = ftok(OVEN_PATH, OVEN_ID)) == -1) {
        perror("ERROR! An error occurred while creating key.\n");
        exit(1);
    }
    if ((oven_shm_id = shmget(oven_key, sizeof(oven), 0)) == -1) {
        perror("ERROR! An error occurred while getting oven id.\n");
        exit(1);
    }
    return oven_shm_id;
}

int get_table_shm_id() {
    key_t table_key;
    int table_shm_id;
    if ((table_key = ftok(TABLE_PATH, TABLE_ID)) == -1) {
        perror("ERROR! An error occurred while creating key.\n");
        exit(1);
    }
    if ((table_shm_id = shmget(table_key, sizeof(table), 0)) == -1) {
        perror("ERROR! An error occurred while getting table id.\n");
        exit(1);
    }
    return table_shm_id;
}

void lock_semaphore(int semaphore_id, int sem_num) {
    struct sembuf sem;
    sem.sem_num = sem_num;
    sem.sem_op = -1;
    if (semop(semaphore_id, &sem, 1) == -1) {
        perror("ERROR! An error occurred while locking semaphore.\n");
        exit(1);
    }
}

void unlock_semaphore(int semaphore_id, int sem_num) {
    struct sembuf sem;
    sem.sem_num = sem_num;
    sem.sem_op = 1;
    if (semop(semaphore_id, &sem, 1) == -1) {
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