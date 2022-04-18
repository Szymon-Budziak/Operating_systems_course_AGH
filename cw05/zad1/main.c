#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

void execute_commands(FILE *file, char **components) {
    char line[LINE_MAX];
    char *component, *curr_component;
    int n_components, j;
    int pipe_first, pipe_last;
    char temp[LINE_MAX], *token, *next_token, *command;
    int curr_fd[2], prev_fd[2];
    int line_commands[LINE_MAX];

    while (fgets(line, LINE_MAX, file) != NULL) {
        printf("\n======================\n");
        n_components = 0;
        pipe_first = 1;
        pipe_last = 0;
        for (int i = 0; line[i] != '\0'; i++) {
            if (i > 0 && line[i] == 'k' && line[i - 1] == 'i' && line[i - 2] == 'n') {
                memset(temp, 0, LINE_MAX);
                i++;
                j = 0;
                while (line[i] != '\0' && line[i] != ' ' && line[i] != '\n') {
                    temp[j++] = line[i++];
                }
                line_commands[n_components] = atoi(temp) - 1;
                n_components++;
            }
        }
        // Executing components
        for (int i = 0; i < n_components; i++) {
            curr_component = components[line_commands[i]];
            component = calloc(strlen(curr_component) + 1, sizeof(char));
            strcpy(component, curr_component);
            token = strtok_r(component, "|", &component);
            printf("Executing command: %s\n", components[line_commands[i]]);
            // Splitting into single commands and executing
            while (token != NULL) {
                if ((next_token = strtok_r(component, "|", &component)) == NULL && i == n_components - 1) {
                    pipe_last = 1;
                    printf("------------\n");
                }
                command = calloc(LINE_MAX, sizeof(char));
                char *args[LINE_MAX];
                int argc = 0;
                char *element = strtok(token, " ");
                strcpy(command, element);
                while (element != NULL && argc < LINE_MAX) {
                    args[argc] = calloc(LINE_MAX, sizeof(char));
                    strcpy(args[argc], element);
                    argc++;
                    element = strtok(NULL, " ");
                }
                args[argc] = NULL;
                pipe(curr_fd);
                if (fork() == 0) {
                    if (pipe_first != 1) {
                        dup2(prev_fd[0], STDIN_FILENO);
                        close(prev_fd[0]);
                    }
                    if (pipe_last != 1)
                        dup2(curr_fd[1], STDOUT_FILENO);
                    if (execvp(command, args) == -1) {
                        perror("execvp error\n");
                        exit(1);
                    }
                }
                close(curr_fd[1]);

                while (wait(NULL) != -1) {
                    if (pipe_first == 1)
                        pipe_first = 0;
                    else
                        close(prev_fd[0]);
                }
                prev_fd[0] = curr_fd[0];
                prev_fd[1] = curr_fd[1];
                // Cleaning up the memory
                free(command);
                for (int k = 0; k < argc; k++)
                    free(args[k]);
                if (next_token != NULL)
                    strcpy(token, next_token);
                else {
                    free(token);
                    token = NULL;
                }
            }
        }
        while (wait(NULL) != -1) {}
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    char *file_path;
    FILE *file;
    file_path = argv[1];
    if ((file = fopen(file_path, "r")) == NULL) {
        perror("Cannot open a file\n");
        exit(1);
    }

    int n_components;
    char line[LINE_MAX];
    n_components = 0;
    while (fgets(line, LINE_MAX, file) != NULL) {
        if (line[0] != 's')
            break;
        else
            n_components++;
    }
    rewind(file);
    char **components = calloc(n_components, sizeof(char *));
    char *token;
    for (int i = 0; i < n_components; i++) {
        if (fgets(line, LINE_MAX, file) != NULL) {
            token = strtok(line, "=");
            token = strtok(NULL, "=");
            components[i] = calloc(strlen(token) + 1, sizeof(char));
            strncpy(components[i], token + 1, strlen(token) - 2);
        } else {
            perror("fgets error\n");
            exit(1);
        }
    }
    execute_commands(file, components);
    for (int i = 0; i < n_components; i++) {
        free(components[i]);
    }
    free(components);
    fclose(file);
}