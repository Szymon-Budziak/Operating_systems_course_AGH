#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define FTW_F 1 /*file other than directory */
#define FTW_D 2 /* directory */

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock;
static char *fullpath;
const char format[] = "%Y-%m-%d %H:%M:%S";

void get_file_type(struct stat *statptr, int type) {
    fprintf(stdout, "%s  |  ", fullpath);
    fprintf(stdout, "%ld  |  ", statptr->st_nlink);

    if (type == FTW_F) {
        if (S_ISREG(statptr->st_mode)) {
            fprintf(stdout, "file  |  ");
            nreg++;
        } else if (S_ISCHR(statptr->st_mode)) {
            fprintf(stdout, "char dev  |  ");
            nchr++;
        } else if (S_ISBLK(statptr->st_mode)) {
            fprintf(stdout, "block dev  |  ");
            nblk++;
        } else if (S_ISFIFO(statptr->st_mode)) {
            fprintf(stdout, "fifo  |  ");
            nfifo++;
        } else if (S_ISLNK(statptr->st_mode)) {
            fprintf(stdout, "slink  |  ");
            nslink++;
        } else if (S_ISSOCK(statptr->st_mode)) {
            fprintf(stdout, "sock  |  ");
            nsock++;
        }
    } else if (type == FTW_D) {
        fprintf(stdout, "dir  |  ");
        ndir++;
    } else {
        fprintf(stderr, "Unknown type %d for pathname %s", type, fullpath);
        exit(1);
    }

    fprintf(stdout, "size %ld  |  ", statptr->st_size);

    struct tm *last_access_time;
    last_access_time = localtime(&statptr->st_atime);
    char last_access_str[256];
    strftime(last_access_str, 256, format, last_access_time);
    fprintf(stdout, "access time %s  |  ", last_access_str);

    struct tm *last_modify_time;
    last_modify_time = localtime(&statptr->st_mtime);
    char last_modify_str[256];
    strftime(last_modify_str, 256, format, last_modify_time);
    fprintf(stdout, "modify time %s\n", last_modify_str);
}

void do_recur_path() {
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int n;

    lstat(fullpath, &statbuf);
    if (S_ISDIR(statbuf.st_mode) == 0) {
        get_file_type(&statbuf, FTW_F);
        return;
    } else
        get_file_type(&statbuf, FTW_D);

    n = strlen(fullpath);
    fullpath[n++] = '/';
    fullpath[n] = 0;

    if ((dp = opendir(fullpath)) == NULL) {
        fprintf(stderr, "Cannot read directory");
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        strcpy(&fullpath[n], dirp->d_name);
        do_recur_path();
    }
    fullpath[n - 1] = 0;
    if (closedir(dp) < 0) {
        fprintf(stderr, "Cannot close directory");
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }

    fullpath = calloc(1000, sizeof(char));
    fullpath = realpath(argv[1], fullpath);
    do_recur_path();

    fprintf(stdout, "regular files: %ld\n", nreg);
    fprintf(stdout, "directories: %ld\n", ndir);
    fprintf(stdout, "block special: %ld\n", nblk);
    fprintf(stdout, "char special: %ld\n", nchr);
    fprintf(stdout, "FIFOs: %ld\n", nfifo);
    fprintf(stdout, "symbolic links: %ld\n", nslink);
    fprintf(stdout, "sockets: %ld\n", nsock);
    free(fullpath);
}