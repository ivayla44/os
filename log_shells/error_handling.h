#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int err_open(const char* path, int flags);
int err_read(int fd, void* buf, size_t count);
int err_write(int fd, void* buf, size_t count);
int err_lseek(int fd, off_t offset, int whence);
int err_waitpid(pid_t pid, int *stat_loc, int options);

#endif