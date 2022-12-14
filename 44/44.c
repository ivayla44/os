#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void pr_uint32(int fd1, int fd2, int fd3) {
    int size = sizeof(uint32_t), check_read = 1, check_write = 1, check_lseek = 1;
    uint32_t x, y, z;
    while(check_read) {
        check_read = read(fd1, &x, size);
        if(!check_read) break;
        check_read = read(fd1, &y, size);
        check_lseek = lseek(fd2, (x - 1) * size, SEEK_SET);
        for(int i = 0; i < y; i++) {
            check_read = read(fd2, &z, size);
            check_write = write(fd3, &z, size);
        }
    }
}

int main(int argc, char* argv[]) {
    char* f1 = argv[1];
    char* f2 = argv[2];
    char* f3 = argv[3];

    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_RDONLY);
    int fd3 = open(f3, O_WRONLY);
    if(fd1 == -1 || fd2 == -1 || fd3 == -1) printf("Fd error."), exit(1);

    pr_uint32(fd1, fd2, fd3);

    close(fd1);
    close(fd2);
    close(fd3);
}
