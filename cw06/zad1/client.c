#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "shared.h"

int server_queue;
int client_id, client_key, client_queue;
pid_t child_pid;

void remove_queue();

void SIGINT_handler(int);

void stop_action();

void to_one_action(int, char *);

void to_all_action(char *);

void list_action();

void handle_input();

int get_receiver_id(char *);

char *get_message_body(char *);

void print_message(msg_buffer);

void handle_msg_control();

int main(int argc, char *argv[]) {
    if (atexit(remove_queue) != 0) {
        perror("ERROR! An error occurred while closing the client queue.\n");
        exit(1);
    }
    if (signal(SIGINT, SIGINT_handler) == SIG_ERR) {
        perror("ERROR! SIGINT signal error.\n");
        exit(1);
    }
    char *home_path;
    key_t server_key;

    if ((home_path = getenv("HOME")) == NULL) {
        perror("ERROR! An error occurred while setting home path.\n");
        exit(1);
    }
    if ((server_key = ftok(home_path, PROJECT_ID)) == -1) {
        perror("ERROR! An error occurred while creating server key.\n");
        exit(1);
    }
    if ((server_queue = msgget(server_key, 0)) == -1) {
        perror("ERROR! An error occurred while creating server queue.\n");
        exit(1);
    }
    if ((client_key = ftok(home_path, getpid())) == -1) {
        perror("ERROR! An error occurred while creating client key.\n");
        exit(1);
    }
    if ((client_queue = msgget(client_key, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        perror("ERROR! An error occurred while creating client queue.\n");
        exit(1);
    }
    printf("Client queue was successfully created with %d key.\n", client_key);

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = stop_action;
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL) != 0) {
        perror("ERROR! Sigaction error.\n");
        exit(1);
    }

    msg_buffer message;
    message.m_type = INIT;
    sprintf(message.m_text, "%d", client_key);
    if (msgsnd(server_queue, &message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending a message from a client.\n");
        exit(1);
    }
    msg_buffer new_message;
    if (msgrcv(client_queue, &new_message, MSG_SIZE, INIT, 0) == -1) {
        perror("ERROR! An error occurred while receiving a message in client.\n");
        exit(1);
    }
    client_id = (int) strtol(new_message.m_text, NULL, 10);
    printf("Client with %d id successfully joined the server.\n", client_id);
    struct msqid_ds msg_control;

    if ((child_pid = fork()) == 0) {
        while (true) {
            if (msgctl(client_queue, IPC_STAT, &msg_control) == -1) {
                perror("ERROR! An error occurred: cannot get information about the client.\n");
            }
            if (msg_control.msg_qnum)
                handle_msg_control();
        }
    } else {
        while (true) {
            handle_input();
        }
    }
}

void remove_queue() {
    msg_buffer message;
    message.m_type = STOP;
    message.sender_id = client_id;
    if (msgsnd(server_queue, &message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending a message from client.\n");
        exit(1);
    }
    if (msgctl(client_queue, IPC_RMID, NULL) == -1) {
        perror("ERROR! An error occurred while removing client queue.\n");
        exit(1);
    }
    printf("Stopped.\n");
}

void SIGINT_handler(int sig_no) {
    printf("Received %d signal. Shutting down the server.\n", sig_no);
    exit(0);
}

void stop_action() {
    msg_buffer message;
    message.m_type = STOP;
    message.sender_id = client_id;
    if (child_pid > 0)
        kill(child_pid, SIGKILL);
    if (msgsnd(server_queue, &message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred: cannot stop a client.\n");
    } else {
        printf("Client was successfully stopped.\n");
    }
    exit(1);
}

void to_one_action(int receiver_id, char *message) {
    printf("Sending ONE message to client with %d id.\n", receiver_id);
    msg_buffer new_message;
    new_message.m_type = _2ONE;
    new_message.sender_id = client_id;
    new_message.receiver_id = receiver_id;
    strcpy(new_message.m_text, message);
    if (msgsnd(server_queue, &new_message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred: cannot send a message to the server queue.\n");
    }
}

void to_all_action(char *message) {
    printf("Sending ALL message to all clients.\n");
    msg_buffer new_message;
    new_message.m_type = _2ALL;
    new_message.sender_id = client_id;
    strcpy(new_message.m_text, message);
    if (msgsnd(server_queue, &new_message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred: cannot send a message to the server.\n");
    }
}

void list_action() {
    printf("Sending LIST message.\n");
    msg_buffer message;
    message.m_type = LIST;
    message.sender_id = client_id;
    if (msgsnd(server_queue, &message, MSG_SIZE, 0) == -1) {
        perror("ERROR! An error occurred while sending a message from a client.\n");
        exit(1);
    }
    msg_buffer new_message;
    if (msgrcv(client_queue, &new_message, MSG_SIZE, LIST, 0) == -1) {
        perror("ERROR! An error occurred: cannot get list of active clients from the server.\n");
        exit(1);
    }
    printf("All active clients: %s.\n", new_message.m_text);
}

void handle_input() {
    char *command, *args;
    char input[LINE_MAX];
    printf("> ");

    if ((fgets(input, sizeof(input), stdin) == NULL)) {
        perror("ERROR! An error occurred while entering the command.\n");
        exit(1);
    }
    command = strtok_r(input, " ", &args);
    if (strncmp(command, "LIST", 4) == 0) {
        list_action();
    } else if (strncmp(command, "2ALL", 4) == 0) {
        char *text;
        if ((text = get_message_body(args)) == NULL)
            stop_action();
        to_all_action(args);
    } else if (strncmp(command, "2ONE", 4) == 0) {
        int receiver_id;
        char *body = get_message_body(args);
        if ((receiver_id = get_receiver_id(args)) < 0 || !(body))
            stop_action();
        to_one_action(receiver_id, body);
    } else if (strncmp(command, "STOP", 4) == 0) {
        stop_action();
    } else
        printf("ERROR! An error occurred: command not found.\n");
}

int get_receiver_id(char *args) {
    char *str_receiver_id;
    char *new_args = "";
    if ((str_receiver_id = strtok_r(args, " \0", &new_args)) == NULL) {
        fprintf(stderr, "ERROR! An error occurred: cannot get receiver id.\n");
        return -1;
    }
    int receiver_id;
    if ((receiver_id = (int) strtol(str_receiver_id, NULL, 10)) < 0 || errno) {
        fprintf(stderr, "ERROR! An error occurred: invalid receiver id.\n");
        return -1;
    }
    strcpy(args, new_args);
    return receiver_id;
}

char *get_message_body(char *args) {
    char *text;
    if ((text = strtok(args, "\n\0")) == NULL) {
        fprintf(stderr, "ERROR! An error occurred: cannot get a message.\n");
        return NULL;
    }
    return text;
}

void print_message(msg_buffer message) {
    time_t send_time;
    struct tm *current_time;
    int year, month, day, hours, minutes, seconds, sender_id;
    char *sender_message;

    send_time = message.send_time;
    current_time = localtime(&send_time);

    year = current_time->tm_year + 1900; // get year since 1900
    month = current_time->tm_mon + 1; // get month of year (0 to 11)
    day = current_time->tm_mday; // get day of month (1 to 31)
    hours = current_time->tm_hour; // get hours since midnight (0-23)
    minutes = current_time->tm_min; // get minutes passed after the hour (0-59)
    seconds = current_time->tm_sec; // get seconds passed after a minute (0-59)
    sender_id = message.sender_id;
    sender_message = message.m_text;

    printf("Date is: %02d/%02d/%d.\nTime is %02d:%02d:%02d.\nMessage send by sender with %d id.\nText of the message: %s\n",
           year, month, day, hours, minutes, seconds, sender_id, sender_message);
}

void handle_msg_control() {
    msg_buffer message;
    if (msgrcv(client_queue, &message, MSG_SIZE, 0, 0) == -1) {
        perror("ERROR! An error occurred: cannot communicate with client queue.\n");
        exit(1);
    }
    switch (message.m_type) {
        case _2ONE:
            printf("Got 2ONE message.\n");
            print_message(message);
            break;
        case _2ALL:
            printf("Got 2ALL message.\n");
            print_message(message);
            break;
        default:
            fprintf(stderr, "ERROR! An error occurred: there is no such type of message.\n> ");
    }
}