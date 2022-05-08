#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <sys/msg.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>

#define MAX_MESSAGE_LENGTH LINE_MAX
#define MAX_CLIENTS 10
#define MAX_CLIENTS_MESSAGES 10
#define MAX_SERVER_MESSAGES 10
#define SERVER_QUEUE_NAME "/server"

typedef struct msg_buffer {
    long m_type;
    char m_text[LINE_MAX];
    int sender_id;
    int receiver_id;
    time_t send_time;
} msg_buffer;

typedef enum msg_type {
    STOP = 1,
    LIST = 2,
    _2ALL = 3,
    _2ONE = 4,
    INIT = 5
} msg_type;

void create_message(msg_buffer message, char *msg) {
    char new_message[MAX_MESSAGE_LENGTH];
    sprintf(new_message, "%ld%s%s%s%d%s%d%s%ld",
            message.m_type,
            "-",
            strlen(message.m_text) ? message.m_text : NULL,
            "-",
            message.sender_id,
            "-",
            message.receiver_id,
            "-",
            message.send_time);
    strcpy(msg, new_message);
    msg[strlen(msg)] = '\0';
}

int msg_send(int client_id, msg_buffer message, int message_type) {
    char msg[MAX_MESSAGE_LENGTH];
    create_message(message, msg);

    if (mq_send(client_id, msg, strlen(msg), message_type) == -1) {
        perror("ERROR! An error occurred cannot send a message to the receiver client.\n");
        return -1;
    }
    return 0;
}

long parse_long(char *message) {
    char *buffer, *number_str;
    long number;
    if ((number_str = strtok_r(message, "-", &buffer)) == NULL) {
        perror("ERROR! An error occurred while parsing message type to long.\n");
        return -1;
    }
    number = strtol(number_str, NULL, 10);
    strcpy(message, buffer);
    if (number == 0 && errno) {
        perror("ERROR! An error occurred while converting string to the number.\n");
        return -1;
    }
    return number;
}

int parse_text(char *target, char *message) {
    char *buffer, *new_message;
    if ((new_message = strtok_r(message, "-", &buffer)) == NULL) {
        perror("ERROR! An error occurred while parsing message.\n");
        return -1;
    }
    strcpy(target, new_message);
    strcpy(message, buffer);
    return 0;
}

msg_buffer *parse_message(char *message) {
    char message_copy[MAX_MESSAGE_LENGTH];
    strcpy(message_copy, message);
    msg_buffer *new_message = calloc(1, sizeof(msg_buffer));
    if ((new_message->m_type = (int) parse_long(message_copy)) == -1
        || parse_text(new_message->m_text, message_copy) == -1
        || (new_message->sender_id = (int) parse_long(message_copy)) == -1
        || (new_message->receiver_id = (int) parse_long(message_copy)) == -1
        || (new_message->send_time = parse_long(message_copy)) == -1) {
        perror("ERROR! An error occurred while parsing a message.\n");
        free(new_message);
        return NULL;
    }
    return new_message;
}

#endif// SHARED_H