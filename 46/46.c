#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void cpy_string(int fd1_1, int fd1_2, int fd2_1, int fd2_2) {
    int check_read = 1, check_write = 1, check_lseek = 1;
    uint16_t start, start2 = 0;
    uint8_t len, resv;
    while(1) {
        check_read = read(fd1_2, &start, sizeof(uint16_t));
        check_read = read(fd1_2, &len, sizeof(uint8_t));
        check_read = read(fd1_2, &resv, sizeof(uint8_t));
        if(!check_read) break;
        check_lseek = lseek(fd1_1, start, SEEK_SET);
        uint8_t temp[len];
        check_read = read(fd1_1, temp, len);
        if (temp[0] >= (uint8_t)'A' && temp[0] <= (uint8_t)'Z') {
            check_lseek = lseek(fd1_1, start - sizeof(uint8_t) - 1, SEEK_SET);
            check_write = write(fd2_1, temp, len * sizeof(uint8_t));
            check_write = write(fd2_2, &start2, sizeof(uint16_t));
            check_write = write(fd2_2, &len, sizeof(uint8_t));
            check_write = write(fd2_2, &resv, sizeof(uint8_t));
            if(!check_write) {
                printf("Write error.");
                break;
            }
            start2 += (uint16_t)len;
        }
    }
}

int main(int argc, char* argv[]) {
    char* f1 = argv[1];
    char* f2 = argv[2];
    char* f3 = argv[3];
    char* f4 = argv[4];

    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_RDONLY);
    int fd3 = open(f3, O_WRONLY);
    int fd4 = open(f4, O_WRONLY);
    if(fd1 == -1 || fd2 == -1 || fd3 == -1 || fd4 == -1) printf("Fd error."), exit(1);

    cpy_string(fd1, fd2, fd3, fd4);

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
}
