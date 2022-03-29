#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

char *fullpath, *substring, *relativepath;
char *command;

void search_in_file() {
    FILE *file;
    size_t file_size;
    char *buffer;
    if ((file = fopen(fullpath, "r")) == NULL) {
        fprintf(stderr, "No such a file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = calloc(file_size + 1, sizeof(char));

    fread(buffer, sizeof(char), file_size + 1, file);
    if (strstr(buffer, substring) != NULL) {
        fprintf(stdout, "%s  |  %d\n", relativepath, getpid());
    }
    free(buffer);
    fclose(file);
}

void search_for_substring(int current_depth) {
    if (current_depth <= 0)
        return;
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int n_full, n_relative;

    n_full = strlen(fullpath);
    fullpath[n_full++] = '/';
    fullpath[n_full] = 0;

    n_relative = strlen(relativepath);
    relativepath[n_relative++] = '/';
    relativepath[n_relative] = 0;

    if ((dp = opendir(fullpath)) == NULL) {
        fprintf(stderr, "Cannot read directory");
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        strcpy(&fullpath[n_full], dirp->d_name);
        strcpy(&relativepath[n_relative], dirp->d_name);
        lstat(fullpath, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (fork() == 0) {
                search_for_substring(current_depth - 1);
                exit(0);
            }
            wait(NULL);
        } else if (S_ISREG(statbuf.st_mode))
            search_in_file();
    }
    fullpath[n_full - 1] = 0;
    relativepath[n_relative - 1] = 0;
    if (closedir(dp) < 0) {
        fprintf(stderr, "Cannot close directory");
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("%d", argc);
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }

    int max_depth;
    fullpath = calloc(1000, sizeof(char));
    fullpath = realpath(argv[1], fullpath);

    substring = calloc(50, strlen(argv[2]) + 1);
    strcpy(substring, argv[2]);
    max_depth = atoi(argv[3]);

    relativepath = calloc(1000, sizeof(char));
    strcpy(relativepath, "..");
    search_for_substring(max_depth);

    free(fullpath);
    free(substring);
    free(relativepath);
}