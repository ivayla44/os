#include "error_handling.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int err_open(const char* path, int flags) {
    int fd = open(path, flags);
    if(fd == -1) {
        printf("Can't open fd.");
    }
    return fd;
}

int err_read(int fd, void* buf, size_t nbyte) {
    int check_read = read(fd, buf, nbyte);
    if(check_read == -1) {
        printf("Reading error.");
        close(fd);
    }
    return check_read;
}

int err_write(int fd, void* buf, size_t nbyte) {
    int check_write = write(fd, buf, nbyte);
    if(check_write == -1) {
        printf("Writing error.");
        close(fd);
    }
    return check_write;
}

int err_lseek(int fd, off_t offset, int whence) {
    int check_lseek = lseek(fd, offset, whence);
    if(check_lseek == -1) {
        printf("Lseek error.");
        close(fd);
    }
    return check_lseek;
}

int err_waitpid(pid_t pid, int *stat_loc, int options) {
    int check_waitpid = waitpid(pid, stat_loc, options);
    if(check_waitpid == -1) {
        printf("Waitpid error.");
    }
    return check_waitpid;
}
