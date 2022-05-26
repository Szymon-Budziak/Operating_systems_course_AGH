#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define ELVES_NUMBER 10
#define REINDEERS_NUMBER 9
#define DELIVERS_NUMBER 3
#define REQUIRED_ELVES_NUMBER 3

pthread_mutex_t sleep_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_task_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_sleeping_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_helping_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_helped_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delivery_ended_cond = PTHREAD_COND_INITIALIZER;

pthread_t santa_thread;
pthread_t reindeer_threads[REINDEERS_NUMBER];
pthread_t elf_threads[ELVES_NUMBER];


int waiting_reindeers = 0;
int waiting_elves = 0;
int deliveries = 0;
int santa_sleeping = 1;
int solving_time = 0;
int *reindeer_id;
int *elf_id;
int elves_queue[REQUIRED_ELVES_NUMBER];
int completed_elves[ELVES_NUMBER] = {0};


void create_threads();

void clean();

void santa_function();

void reindeer_function(int *);

void elf_function(int *);

void cleanup_elf(void *arg);

int main(int argc, char *argv[]) {
    create_threads();
    clean();
}

void create_threads() {
    pthread_create(&santa_thread, NULL, (void *(*)(void *)) santa_function, NULL);

    reindeer_id = calloc(REINDEERS_NUMBER, sizeof(int));
    for (int i = 0; i < REINDEERS_NUMBER; i++) {
        reindeer_id[i] = i + 1;
        pthread_create(&reindeer_threads[i], NULL, (void *(*)(void *)) reindeer_function, &reindeer_id[i]);
    }

    elf_id = calloc(ELVES_NUMBER, sizeof(int));
    for (int i = 0; i < ELVES_NUMBER; i++) {
        elf_id[i] = i + 1;
        pthread_create(&elf_threads[i], NULL, (void *(*)(void *)) elf_function, &elf_id[i]);
    }
}

void clean() {
    pthread_join(santa_thread, NULL);

    for (int i = 0; i < REINDEERS_NUMBER; i++)
        pthread_join(reindeer_threads[i], NULL);
    for (int i = 0; i < ELVES_NUMBER; i++)
        pthread_cancel(elf_threads[i]);

    while (1) {
        int not_done;
        not_done = false;
        for (int i = 0; i < ELVES_NUMBER; i++) {
            if (completed_elves[i] != 1)
                not_done = true;
        }
        if (not_done == false) {
            pthread_mutex_destroy(&sleep_mutex);
            pthread_mutex_destroy(&reindeers_mutex);
            pthread_mutex_destroy(&elves_mutex);
            pthread_mutex_destroy(&reindeers_waiting_mutex);
            pthread_mutex_destroy(&elves_waiting_mutex);

            pthread_cond_destroy(&santa_task_cond);
            pthread_cond_destroy(&santa_sleeping_cond);
            pthread_cond_destroy(&santa_helping_cond);
            pthread_cond_destroy(&delivery_ended_cond);

            free(elf_id);
            free(reindeer_id);
            exit(0);
        }
    }
}

void santa_function() {
    srand(pthread_self());
    while (1) {
        pthread_mutex_lock(&sleep_mutex);
        pthread_cond_wait(&santa_task_cond, &sleep_mutex);
        santa_sleeping = 0;
        pthread_mutex_unlock(&sleep_mutex);

        printf("Santa claus: I am waking up.\n");

        pthread_mutex_lock(&reindeers_mutex);
        if (waiting_reindeers == REINDEERS_NUMBER) {
            printf("Santa claus: I am delivering toys for the %d time.\n", deliveries + 1);
            usleep((rand() % 2001 + 2000) * 1000);
            deliveries++;
            pthread_cond_broadcast(&delivery_ended_cond);
            waiting_reindeers = 0;
            pthread_mutex_unlock(&reindeers_mutex);
            if (deliveries == DELIVERS_NUMBER) {
                printf("Santa claus: I have delivered everything.\n");
                pthread_exit((void *) 0);
            }
        } else {
            pthread_mutex_unlock(&reindeers_mutex);
            printf("Santa claus: I am solving elves problem: %d %d %d.\n", elves_queue[0], elves_queue[1],
                   elves_queue[2]);
            solving_time = (rand() % 1001 + 1000) * 1000;
            pthread_mutex_lock(&elves_mutex);
            pthread_cond_broadcast(&santa_helping_cond);
            pthread_mutex_unlock(&elves_mutex);
            usleep(solving_time);
            printf("Santa claus: I have solved elves problem: %d %d %d.\n", elves_queue[0], elves_queue[1],
                   elves_queue[2]);
            pthread_mutex_lock(&elves_mutex);
            pthread_cond_broadcast(&santa_helping_cond);
            waiting_elves = 0;
            pthread_mutex_unlock(&elves_mutex);
        }
        pthread_mutex_lock(&sleep_mutex);
        santa_sleeping = 1;
        printf("Santa claus: I am going to sleep.\n");
        pthread_cond_broadcast(&santa_sleeping_cond);
        pthread_mutex_unlock(&sleep_mutex);
    }
}

void reindeer_function(int *id) {
    srand((*id + getpid()) << 16);
    while (deliveries < DELIVERS_NUMBER) {
        usleep((rand() % 5001 + 5000) * 1000);
        pthread_mutex_lock(&reindeers_mutex);
        waiting_reindeers++;
        printf("Reindeer [ID: %d]: I am waiting for Santa Claus. Currently %d reindeers are waiting.\n", *id,
               waiting_reindeers);
        if (waiting_reindeers == REINDEERS_NUMBER) {
            pthread_mutex_unlock(&reindeers_mutex);
            pthread_mutex_lock(&sleep_mutex);
            while (santa_sleeping == 0)
                pthread_cond_wait(&santa_sleeping_cond, &sleep_mutex);
            pthread_mutex_unlock(&sleep_mutex);
            printf("Reindeer [ID: %d]: I am waking up Santa Claus.\n", *id);
            pthread_cond_broadcast(&santa_task_cond);
        } else
            pthread_mutex_unlock(&reindeers_mutex);
        pthread_mutex_lock(&reindeers_waiting_mutex);
        pthread_cond_wait(&delivery_ended_cond, &reindeers_waiting_mutex);
        pthread_mutex_unlock(&reindeers_waiting_mutex);
    }
    pthread_exit((void *) 0);
}

void elf_function(int *id) {
    srand((*id + getpid()) << 16);
    pthread_cleanup_push(cleanup_elf, id) ;
            while (1) {
                int first_wait = true;
                usleep((rand() % 3001 + 2000) * 1000);
                pthread_mutex_lock(&elves_waiting_mutex);
                while (waiting_elves == REQUIRED_ELVES_NUMBER) {
                    if (first_wait) {
                        printf("Elf [ID %d]: I am waiting for the return of the elves.\n", *id);
                        first_wait = false;
                    }
                    pthread_cond_wait(&santa_helped_cond, &elves_waiting_mutex);
                }
                first_wait = true;
                pthread_mutex_lock(&elves_mutex);
                waiting_elves++;
                pthread_mutex_unlock(&elves_waiting_mutex);
                elves_queue[waiting_elves - 1] = *id;
                printf("Elf [ID %d]: I am waiting for Santa Claus. Currently %d elves are waiting.\n", *id,
                       waiting_elves);
                if (waiting_elves == REQUIRED_ELVES_NUMBER) {
                    pthread_mutex_unlock(&elves_mutex);
                    pthread_mutex_lock(&sleep_mutex);
                    pthread_mutex_lock(&reindeers_mutex);
                    while (santa_sleeping == 0 || waiting_reindeers == REINDEERS_NUMBER) {
                        pthread_mutex_unlock(&reindeers_mutex);
                        pthread_cond_wait(&santa_sleeping_cond, &sleep_mutex);
                        pthread_mutex_lock(&reindeers_mutex);
                    }
                    pthread_mutex_unlock(&reindeers_mutex);
                    printf("Elf [ID %d]: I am waking up Santa Claus.\n", *id);
                    pthread_mutex_lock(&elves_mutex);
                    pthread_cond_broadcast(&santa_task_cond);
                    pthread_mutex_unlock(&sleep_mutex);
                }
                pthread_cond_wait(&santa_helping_cond, &elves_mutex);
                pthread_mutex_unlock(&elves_mutex);
                printf("Elf [ID %d]: Santa Claus is solving the problems.\n", *id);
                usleep(solving_time);
            }
    pthread_cleanup_pop(1);
}

void cleanup_elf(void *arg) {
    int *id = arg;
    pthread_mutex_unlock(&elves_waiting_mutex);
    pthread_mutex_unlock(&sleep_mutex);
    pthread_mutex_unlock(&elves_mutex);
    completed_elves[*id - 1] = 1;
}