#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void sort_emails(char *order_mode) {
    FILE *file;
    char *command, line[LINE_MAX];
    printf("Printing emails ordered by %s\n", order_mode);
    if (strcmp(order_mode, "sender") == 0) {
        command = "echo | mail | tail -n +2 | head -n -1 | sort -k 2";
    } else if (strcmp(order_mode, "date") == 0) {
        command = "echo | mail | tail -n +2 | head -n -1 | tac";
    } else {
        perror("Wrong mode. Mode order should be sender or date\n");
        exit(1);
    }
    if ((file = popen(command, "r")) == NULL) {
        perror("popen error\n");
        exit(1);
    }
    while (fgets(line, LINE_MAX, file) != NULL) {
        printf("%s\n", line);
    }
    pclose(file);
}

void send_email(char *address, char *title, char *content) {
    FILE *file;
    char command[LINE_MAX];
    sprintf(command, "echo %s | mail -s %s %s", content, title, address);
    if ((file = popen(command, "r")) == NULL) {
        perror("popen error\n");
        exit(1);
    }
    printf("Sending email to: %s\nwith title: %s\nand content:\n%s", address, title, content);
    pclose(file);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        char *order_mode;
        order_mode = argv[1];
        sort_emails(order_mode);
    } else if (argc == 4) {
        char *address, *title, *content;
        address = argv[1];
        title = argv[2];
        content = argv[3];
        send_email(address, title, content);
    } else {
        perror("Wrong number of arguments\n");
        exit(1);
    }
}