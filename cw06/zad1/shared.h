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

#define MAX_CLIENTS 10
#define PROJECT_ID 2

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

const size_t MSG_SIZE = sizeof(msg_buffer) - sizeof(long);
#endif// SHARED_H