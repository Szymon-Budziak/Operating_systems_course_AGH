#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ftw.h>
#include <time.h>

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock;
const char format[] = "%Y-%m-%d %H:%M:%S";

void get_file_status(const char *pathname, const struct stat *statptr, int type, struct FTW *pfwt) {
    fprintf(stdout, "%s  |  ", pathname);
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
        fprintf(stderr, "Unknown type %d for pathname %s", type, pathname);
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


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments");
        exit(1);
    }
    int flags;
    void *fn;
    flags = FTW_PHYS;
    fn = get_file_status;

    nftw(argv[1], fn, 2, flags);

    fprintf(stdout, "regular files: %ld\n", nreg);
    fprintf(stdout, "directories: %ld\n", ndir);
    fprintf(stdout, "block special: %ld\n", nblk);
    fprintf(stdout, "char special: %ld\n", nchr);
    fprintf(stdout, "FIFOs: %ld\n", nfifo);
    fprintf(stdout, "symbolic links: %ld\n", nslink);
    fprintf(stdout, "sockets: %ld\n", nsock);
}