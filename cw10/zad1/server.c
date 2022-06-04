#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256

typedef struct {
    char *name;
    int fd;
    int online;
} client;

client *clients[MAX_PLAYERS] = {NULL};
int clients_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void exit_on_error(char *);

int init_local_socket(char *);

int init_network_socket(char *);

void ping();

int add_client(char *, int);

void remove_client(char *);

int check_messages(int, int);

int get_by_name(char *);

int get_opponent(int);

int main(int argc, char *argv[]) {
    if (argc != 3)
        exit_on_error("./server [port] [path]");
    char *port, *socket_path;
    int local_socket, network_socket;
    port = argv[1];
    socket_path = argv[2];
    srand(time(NULL));
    local_socket = init_local_socket(socket_path);
    network_socket = init_network_socket(port);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *(*)(void *)) ping, NULL);
    while (1) {
        int client_fd = check_messages(local_socket, network_socket);
        char buffer[MAX_MESSAGE_LENGTH + 1];
        recv(client_fd, buffer, MAX_MESSAGE_LENGTH, 0);
        printf("%s\n", buffer);
        char *command, *arg, *name;
        command = strtok(buffer, ":");
        arg = strtok(NULL, ":");
        name = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0) {
            int index = add_client(name, client_fd);
            if (index == -1) {
                send(client_fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0);
                close(client_fd);
            } else if (index % 2 == 0)
                send(client_fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0);
            else {
                int random_num = rand() % 100;
                int first, second;
                if (random_num % 2 == 0) {
                    first = index;
                    second = get_opponent(index);
                } else {
                    second = index;
                    first = get_opponent(index);
                }
                send(clients[first]->fd, "add:O", MAX_MESSAGE_LENGTH, 0);
                send(clients[second]->fd, "add:X", MAX_MESSAGE_LENGTH, 0);
            }
        }
        if (strcmp(command, "move") == 0) {
            int move = atoi(arg);
            int player = get_by_name(name);
            sprintf(buffer, "move:%d", move);
            send(clients[get_opponent(player)]->fd, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        if (strcmp(command, "quit") == 0)
            remove_client(name);
        if (strcmp(command, "pong") == 0) {
            int player = get_by_name(name);
            if (player != -1)
                clients[player]->online = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
}

void exit_on_error(char *mess) {
    fprintf(stderr, "ERROR: %s.\n", mess);
    fprintf(stderr, "ERRNO: %d.\n", errno);
    exit(1);
}

int init_local_socket(char *path) {
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);
    unlink(path);
    bind(local_socket, (struct sockaddr *) &local_sockaddr, sizeof(struct sockaddr_un));
    listen(local_socket, MAX_BACKLOG);
    return local_socket;
}

int init_network_socket(char *port) {
    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &info);
    int network_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(network_socket, info->ai_addr, info->ai_addrlen);
    listen(network_socket, MAX_BACKLOG);
    freeaddrinfo(info);
    return network_socket;
}

void ping() {
    while (1) {
        printf("*PINGING*\n");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (clients[i] != NULL && !clients[i]->online)
                remove_client(clients[i]->name);
        }
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (clients[i] != NULL) {
                send(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0);
                clients[i]->online = 0;
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(3);
    }
}

int add_client(char *name, int fd) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return -1;
    }
    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i += 2) {
        if (clients[i] != NULL && clients[i + 1] == NULL) {
            index = i + 1;
            break;
        }
    }
    if (index == -1) {
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (clients[i] == NULL) {
                index = i;
                break;
            }
        }
    }
    if (index != -1) {
        client *new_client = calloc(1, sizeof(client));
        new_client->name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(new_client->name, name);
        new_client->fd = fd;
        new_client->online = 1;
        clients[index] = new_client;
        clients_count++;
    }
    return index;
}

void remove_client(char *name) {
    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            index = i;
    }
    printf("Removing client: %s.\n", name);
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    clients_count--;
    int opponent = get_opponent(index);
    if (clients[opponent] != NULL) {
        printf("Removing opponent: %s.\n", clients[opponent]->name);
        free(clients[opponent]->name);
        free(clients[opponent]);
        clients[opponent] = NULL;
        clients_count--;
    }
}

int check_messages(int local_socket, int network_socket) {
    struct pollfd *fds = calloc(2 + clients_count, sizeof(struct pollfd));
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clients_count; i++) {
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(fds, clients_count + 2, -1);
    int result;
    result = 0;
    for (int i = 0; i < clients_count + 2; i++) {
        if (fds[i].revents & POLLIN) {
            result = fds[i].fd;
            break;
        }
    }
    if (result == local_socket || result == network_socket)
        result = accept(result, NULL, NULL);
    free(fds);
    return result;
}

int get_by_name(char *name) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return i;
    }
    return -1;
}

int get_opponent(int index) {
    if (index % 2 == 0)
        return index + 1;
    else
        return index - 1;
}