#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 256

typedef enum {
    FREE,
    O,
    X
} object;

typedef struct {
    int move;
    object objects[9];
} Board;

typedef enum {
    GAME_STARTING,
    WAITING,
    WAITING_FOR_MOVE,
    OPPONENT_MOVE,
    MOVE,
    QUIT
} State;

Board board;
State state = GAME_STARTING;
char *command, *arg, *name;
int server_socket, binded_socket, is_client_O;
char buffer[MAX_MESSAGE_LENGTH + 1];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void quit();

void init_server_connection(char *, int);

void listen_server(int);

void play_game();

Board new_board();

int move(int);

void draw();

void check_game();

object check_winner();

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Wrong number of arguments.\n");
        exit(1);
    }
    char *type, *destination;
    int is_local;
    name = argv[1];
    type = argv[2];
    destination = argv[3];
    signal(SIGINT, quit);

    is_local = strcmp(type, "local") == 0;
    init_server_connection(destination, is_local);
    listen_server(is_local);
}

void quit() {
    char temp_buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(temp_buffer, "quit: :%s", name);
    send(server_socket, temp_buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

void init_server_connection(char *destination, int is_local) {
    struct sockaddr_un local_sockaddr;
    if (is_local) {
        server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);
        connect(server_socket, (struct sockaddr *) &local_sockaddr, sizeof(struct sockaddr_un));
        binded_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un binded_sockaddr;
        memset(&binded_sockaddr, 0, sizeof(struct sockaddr_un));
        binded_sockaddr.sun_family = AF_UNIX;
        sprintf(binded_sockaddr.sun_path, "%d", getpid());
        bind(binded_socket, (struct sockaddr *) &binded_sockaddr, sizeof(struct sockaddr_un));
    } else {
        struct addrinfo *info;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        getaddrinfo("127.0.0.1", destination, &hints, &info);
        server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        connect(server_socket, info->ai_addr, info->ai_addrlen);
        freeaddrinfo(info);
    }
    char temp_buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(temp_buffer, "add: :%s", name);
    if (is_local)
        sendto(binded_socket, temp_buffer, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *) &local_sockaddr,
               sizeof(struct sockaddr_un));
    else
        send(server_socket, temp_buffer, MAX_MESSAGE_LENGTH, 0);
}

void listen_server(int is_local) {
    int game_thread_running = 0;
    while (1) {
        if (is_local)
            recv(binded_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        else
            recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        command = strtok(buffer, ":");
        arg = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0) {
            state = GAME_STARTING;
            if (!game_thread_running) {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *)) play_game, NULL);
                game_thread_running = 1;
            }
        } else if (strcmp(command, "move") == 0)
            state = OPPONENT_MOVE;
        else if (strcmp(command, "quit") == 0) {
            state = QUIT;
            exit(0);
        } else if (strcmp(command, "ping") == 0) {
            sprintf(buffer, "pong: :%s", name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

void play_game() {
    while (1) {
        if (state == GAME_STARTING) {
            if (strcmp(arg, "name_taken") == 0) {
                perror("Name is already taken.\n");
                exit(1);
            } else if (strcmp(arg, "no_enemy") == 0) {
                printf("Waiting for opponent.\n");
                state = WAITING;
            } else {
                board = new_board();
                is_client_O = arg[0] == 'O';
                state = is_client_O ? MOVE : WAITING_FOR_MOVE;
            }
        } else if (state == WAITING) {
            pthread_mutex_lock(&mutex);
            while (state != GAME_STARTING && state != QUIT)
                pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
            board = new_board();
            is_client_O = arg[0] == 'O';
            state = is_client_O ? MOVE : WAITING_FOR_MOVE;
        } else if (state == WAITING_FOR_MOVE) {
            printf("Waiting for opponent's move.\n");
            pthread_mutex_lock(&mutex);
            while (state != OPPONENT_MOVE && state != QUIT)
                pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        } else if (state == OPPONENT_MOVE) {
            int pos = atoi(arg);
            move(pos);
            check_game();
            if (state != QUIT)
                state = MOVE;
        } else if (state == MOVE) {
            draw();
            int pos;
            do {
                printf("Next move (%c): ", is_client_O ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(pos));
            draw();
            char temp_buffer[MAX_MESSAGE_LENGTH + 1];
            sprintf(temp_buffer, "move:%d:%s", pos, name);
            send(server_socket, temp_buffer, MAX_MESSAGE_LENGTH, 0);
            check_game();
            if (state != QUIT)
                state = WAITING_FOR_MOVE;
        } else if (state == QUIT)
            quit();
    }
}

Board new_board() {
    Board new_board = {1, {FREE}};
    return new_board;
}

int move(int position) {
    if (position < 0 || position > 9 || board.objects[position] != FREE)
        return 0;
    board.objects[position] = board.move ? O : X;
    board.move = !board.move;
    return 1;
}

void draw() {
    char symbol;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (board.objects[y * 3 + x] == FREE)
                symbol = y * 3 + x + 1 + '0';
            else if (board.objects[y * 3 + x] == O)
                symbol = 'O';
            else
                symbol = 'X';
            printf("  %c  ", symbol);
        }
        printf("\n_________________________\n");
    }
}

void check_game() {
    int win = 0;
    object winner = check_winner();
    if (winner != FREE) {
        if ((is_client_O && winner == O) || (!is_client_O && winner == X))
            printf("WIN!\n");
        else
            printf("LOST!\n");
        win = 1;
    }
    int draw = 1;
    for (int i = 0; i < 9; i++) {
        if (board.objects[i] == FREE) {
            draw = 0;
            break;
        }
    }
    if (draw && !win)
        printf("DRAW!\n");
    if (win || draw)
        state = QUIT;
}

object check_winner() {
    object column = FREE;
    for (int x = 0; x < 3; x++) {
        object first = board.objects[x];
        object second = board.objects[x + 3];
        object third = board.objects[x + 6];
        if (first == second && first == third && first != FREE)
            column = first;
    }
    if (column != FREE)
        return column;
    object row = FREE;
    for (int y = 0; y < 3; y++) {
        object first = board.objects[3 * y];
        object second = board.objects[3 * y + 1];
        object third = board.objects[3 * y + 2];
        if (first == second && first == third && first != FREE)
            row = first;
    }
    if (row != FREE)
        return row;
    object lower_diagonal = FREE;
    object first = board.objects[0];
    object second = board.objects[4];
    object third = board.objects[8];
    if (first == second && first == third && first != FREE)
        lower_diagonal = first;
    if (lower_diagonal != FREE)
        return lower_diagonal;
    object upper_diagonal = FREE;
    first = board.objects[2];
    second = board.objects[4];
    third = board.objects[6];
    if (first == second && first == third && first != FREE)
        upper_diagonal = first;
    return upper_diagonal;
}