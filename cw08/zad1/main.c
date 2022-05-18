#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

int **image, **negative_image;
int threads_number;
int width, height;

void load_picture(char *);

void *numbers_method(void *);

void *block_method(void *);

void save_to_times(char *, pthread_t *, struct timeval);

void save_negative_picture(char *);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        perror("Wrong number of arguments.\n");
        exit(1);
    }
    char *method, *input_file, *output_file;
    threads_number = atoi(argv[1]);
    method = argv[2];
    if ((strcmp(method, "numbers") != 0) && (strcmp(method, "block") != 0)) {
        perror("Error! Wrong method name.\n");
        exit(1);
    }
    input_file = argv[3];
    output_file = argv[4];

    load_picture(input_file);

    negative_image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++)
        negative_image[i] = calloc(width, sizeof(int));

    pthread_t *threads;
    int *threads_index;
    threads = calloc(threads_number, sizeof(pthread_t));
    threads_index = calloc(threads_number, sizeof(int));

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < threads_number; i++) {
        threads_index[i] = i;
        if (strcmp(method, "numbers") == 0)
            pthread_create(&threads[i], NULL, &numbers_method, &threads_index[i]);
        else if (strcmp(method, "block") == 0)
            pthread_create(&threads[i], NULL, &block_method, &threads_index[i]);
    }

    save_to_times(method, threads, start);
    save_negative_picture(output_file);

    for (int i = 0; i < height; i++) {
        free(image[i]);
        free(negative_image[i]);
    }
    free(image);
    free(negative_image);
}

void load_picture(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        perror("Error! An error occurred while opening input file.\n");
        exit(1);
    }
    char *buffer;
    bool get_max_pixel, read_image;

    buffer = calloc(LINE_MAX, sizeof(char));
    read_image = false;
    if (fgets(buffer, LINE_MAX, file) == NULL) {
        perror("Error! An error occurred with fgets method.\n");
        exit(1);
    }
    while (!read_image && fgets(buffer, LINE_MAX, file)) {
        if (buffer[0] == '#')
            continue;
        else if (!get_max_pixel) {
            sscanf(buffer, "%d %d\n", &width, &height);
            printf("width: %d, height: %d\n", width, height);
            get_max_pixel = true;
        } else {
            int max_pixel;
            sscanf(buffer, "%d \n", &max_pixel);
            printf("M: %d\n", max_pixel);
            if (max_pixel != 255) {
                perror("Error! An error occurred with max grey value which must be 255.\n");
                exit(1);
            }
            read_image = true;
        }
    }
    image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++)
        image[i] = calloc(width, sizeof(int));

    int pixel;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &pixel);
            image[i][j] = pixel;
        }
    }
    fclose(file);
}

void *numbers_method(void *arg) {
    struct timeval start, end;
    int index, to, from, pixel;

    gettimeofday(&start, NULL);
    index = *((int *) arg);

    from = 256 / threads_number * index;
    if (index != threads_number - 1)
        to = 256 / threads_number * (index + 1);
    else
        to = 256;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            pixel = image[i][j];
            if (pixel >= from && pixel < to)
                negative_image[i][j] = 255 - pixel;
        }
    }
    gettimeofday(&end, NULL);
    long int *time;
    time = malloc(sizeof(long int));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    pthread_exit(time);
}

void *block_method(void *arg) {
    struct timeval start, end;
    int index, to, from, pixel;

    gettimeofday(&start, NULL);
    index = *((int *) arg);

    from = index * ceil(width / threads_number);
    if (index != threads_number - 1)
        to = (index + 1) * ceil(width / threads_number) - 1;
    else
        to = width - 1;

    for (int i = 0; i < height; i++) {
        for (int j = from; j <= to; j++) {
            pixel = image[i][j];
            negative_image[i][j] = 255 - pixel;
        }
    }
    gettimeofday(&end, NULL);
    long int *time;
    time = malloc(sizeof(long int));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    pthread_exit(time);
}

void save_to_times(char *method, pthread_t *threads, struct timeval start) {
    struct timeval end;
    FILE *times_file;
    if ((times_file = fopen("Times.txt", "a")) == NULL) {
        perror("Error! An error occurred while appending to a file.\n");
        exit(1);
    };
    printf("-----------------------------\n");
    printf("Number of threads: %d\n", threads_number);
    printf("Method name: %s\n", method);

    fprintf(times_file, "-----------------------------\n");
    fprintf(times_file, "Number of threads: %d\n", threads_number);
    fprintf(times_file, "Method name: %s\n", method);
    for (int i = 0; i < threads_number; i++) {
        long int *time;
        pthread_join(threads[i], (void **) &time);
        printf("Thread number: %d\tTime: %lu\n", i, *time);
        fprintf(times_file, "Thread number: %d\tTime: %lu\n", i, *time);
    }
    gettimeofday(&end, NULL);
    long int *time;
    time = malloc(sizeof(long int));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Total time is: %lu μs\n", *time);
    fprintf(times_file, "Total time is: %lu μs\n", *time);
    fclose(times_file);
    free(time);
}

void save_negative_picture(char *filename) {
    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) {
        perror("Error! An error occurred while opening a file.\n");
        exit(1);
    }
    fprintf(file, "P2\n");
    fprintf(file, "# Processed picture\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file, "%d ", negative_image[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}