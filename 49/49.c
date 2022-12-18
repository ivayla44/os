#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void frompatch(int fd1, int fd2, int fdpatch) {
    int check_read = 1, check_write = 1, check_lseek = 1;
    uint16_t start, start2 = 0;
    uint8_t new;
    check_read = read(fdpatch, &start, sizeof(uint16_t));
    while(1) {
        check_read = read(fd1, &new, sizeof(uint8_t));
        if(!check_read)break;
        if(start == start2) {
// REMARK: Smart.
            check_lseek = lseek(fdpatch, sizeof(uint8_t), SEEK_CUR);
            check_read = read(fdpatch, &new, sizeof(uint8_t));
            check_read = read(fdpatch, &start, sizeof(uint16_t));
        }
        check_write = write(fd2, &new, sizeof(uint8_t));
        start2++;
    }
}

int main(int argc, char* argv[]) {
    char* f1 = argv[1];
    char* f2 = argv[2];
    char* f3 = argv[3];

    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_WRONLY | O_TRUNC);
    int fd3 = open(f3, O_RDONLY);
    if(fd1 == -1 || fd2 == -1 || fd3 == -1) printf("Fd error."), exit(1);

    frompatch(fd1, fd2, fd3);

    close(fd1);
    close(fd2);
    close(fd3);
}
