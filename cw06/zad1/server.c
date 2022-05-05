#include "shared.h"

int connected_clients[MAX_CLIENTS] = {0};
char *filepath = "server_records.txt";

void close_queue();

void SIGINT_handler(int);

void handle_message(msg_buffer);

int save_message(msg_buffer);

void list_action(msg_buffer);

void to_all_action(msg_buffer);

void to_one_action(int, int, char *);

void stop_action(msg_buffer);

void init_action(msg_buffer);

int main(int argc, char *argv[]) {
    if (atexit(close_queue) != 0) {
        perror("ERROR! An error occurred while closing the server queue.\n");
        exit(1);
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIGINT_handler;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("ERROR! Sigaction error.\n");
        exit(1);
    }

    char *home_path;
    key_t key;
    int server_queue;

    if ((home_path = getenv("HOME")) == NULL) {
        perror("ERROR! An error occurred while setting home path.\n");
        exit(1);
    }
    if ((key = ftok(home_path, PROJECT_ID)) == -1) {
        perror("ERROR! An error occurred while creating key.\n");
        exit(1);
    }
    if ((server_queue = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("ERROR! An error occurred while creating server queue.\n");
        exit(1);
    }
    printf("Server is ready! Server queue started with %d key.\n", key);
    msg_buffer message;
    while (true) {
        if (msgrcv(server_queue, &message, MSG_SIZE, -(INIT + 1), 0) == -1) {
            perror("ERROR! An error occurred while getting a message from server queue.\n");
            exit(1);
        }
        handle_message(message);
    }
}

void close_queue() {
    msg_buffer message;
    message.m_type = STOP;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (connected_clients[i] == 0)
            continue;
        else if (msgsnd(connected_clients[i], &message, MSG_SIZE, 0) == -1)
            fprintf(stderr, "Cannot send STOP message to client with %d id.\n", i);
        else
            printf("STOP message successfully sent to client with % id.\n", i);
    }
}

void SIGINT_handler(int sig_no) {
    printf("Received %d signal. Shutting down the server.\n", sig_no);
    exit(0);
}

void handle_message(msg_buffer message) {
    if (save_message(message) == -1) {
        perror("ERROR! An error occurred while saving a message to a file.\n");
        exit(1);
    }
    switch (message.m_type) {
        case LIST:
            printf("Server got LIST signal from sender with %d id.\n", message.sender_id);
            list_action(message);
            break;
        case _2ALL:
            printf("Server got 2ALL signal from sender with %d id.\n", message.sender_id);
            to_all_action(message);
            break;
        case _2ONE:
            printf("Server got 2ONE signal from sender with %d id.\n", message.sender_id);
            to_one_action(message.sender_id, message.receiver_id, message.m_text);
            break;
        case STOP:
            printf("Server got STOP signal from sender with %d id.\n", message.sender_id);
            stop_action(message);
            break;
        case INIT:
            printf("Server got INIT signal from sender with %d id.\n", message.sender_id);
            init_action(message);
            break;
        default:
            perror("ERROR! An error occurred: unknown type of message in server queue.\n");
            break;
    }
}

int save_message(msg_buffer message) {
    FILE *file;
    if ((file = fopen(filepath, "a")) == NULL)
        return -1;
    time_t raw_time;
    struct tm *current_time;
    int year, month, day, hours, minutes, seconds, sender_id;
    char *sender_message;

    time(&raw_time);
    current_time = localtime(&raw_time);

    year = current_time->tm_year + 1900; // get year since 1900
    month = current_time->tm_mon + 1; // get month of year (0 to 11)
    day = current_time->tm_mday; // get day of month (1 to 31)
    hours = current_time->tm_hour; // get hours since midnight (0-23)
    minutes = current_time->tm_min; // get minutes passed after the hour (0-59)
    seconds = current_time->tm_sec; // get seconds passed after a minute (0-59)
    sender_id = message.sender_id;
    sender_message = message.m_text;

    fprintf(file,
            "Date is: %02d/%02d/%d.\nTime is %02d:%02d:%02d.\nMessage send by sender with %d id.\nText of the message: %s\n-----------------------\n",
            year, month, day, hours, minutes, seconds, sender_id, sender_message);
    fclose(file);
    return 0;
}

void list_action(msg_buffer message) {
    char *text;
    char buffer[32];
    text = calloc(LINE_MAX, sizeof(char));
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (connected_clients[i]) {
            printf("Client id %d.", i);
            sprintf(buffer, "%d\n", i);
            strcat(text, buffer);
        }
    }
    msg_buffer new_message;
    new_message.m_type = LIST;
    strcpy(new_message.m_text, text);
    if (msgsnd(connected_clients[message.sender_id], &new_message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending a message from server.\n");
        exit(1);
    }
    printf("List successfully displayed.\n");
    free(text);
}

void to_all_action(msg_buffer message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (connected_clients[i] && i != message.sender_id)
            to_one_action(message.sender_id, i, message.m_text);
    }
    printf("Message sent to all other clients by client with %d id.\n", message.sender_id);
}

void to_one_action(int sender_id, int receiver_id, char *message) {
    if (connected_clients[receiver_id] == 0) {
        fprintf(stderr, "ERROR! An error occurred: There is no client with %d. Cannot send a message.\n", receiver_id);
        exit(1);
    }
    msg_buffer new_message;
    new_message.m_type = _2ONE;
    new_message.sender_id = sender_id;
    new_message.receiver_id = receiver_id;
    new_message.send_time = time(0);
    strcpy(new_message.m_text, message);
    if (msgsnd(connected_clients[receiver_id], &new_message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending a message to client.\n");
        exit(1);
    }
    printf("Message successfully sent to client with %d id.\n", receiver_id);
}

void stop_action(msg_buffer message) {
    if (connected_clients[message.sender_id] == 0) {
        fprintf(stderr, "ERROR! An error occurred: there is no such a client.\n");
    }
    connected_clients[message.sender_id] = 0;
    printf("Client %d has left the server.\n", message.sender_id);
}

void init_action(msg_buffer message) {
    int new_id, found_free_id;
    key_t new_client_key;
    found_free_id = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (connected_clients[i] == 0) {
            found_free_id = 1;
            new_id = i;
        }
    }
    if (found_free_id == 0) {
        fprintf(stderr, "ERROR! An error occurred: cannot connect client to a server. All slots are taken.\n");
        return;
    }
    new_client_key = (key_t) strtol(message.m_text, NULL, 10);
    if ((connected_clients[new_id] = msgget(new_client_key, 0)) == -1) {
        fprintf(stderr, "ERROR! An error occurred: cannot get client queue identifier.\n");
    }
    msg_buffer new_message;
    new_message.m_type = INIT;
    new_message.sender_id = new_id;
    sprintf(new_message.m_text, "%d", new_id);

    if (msgsnd(connected_clients[new_id], &new_message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending message from server.\n");
        exit(1);
    }
    printf("Client with %d id has just joined the server.\n", new_id);
}