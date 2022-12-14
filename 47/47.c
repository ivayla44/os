#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void patch(int fd1, int fd2, int fdpatch) {
    int check_read = 1, check_write = 1;
    uint16_t start = 0;
    uint8_t first, second;
    while(1) {
        check_read = read(fd1, &first, sizeof(uint8_t));
        check_read = read(fd2, &second, sizeof(uint8_t));
        if(!check_read) break;
        if(first != second) {
            check_write = write(fdpatch, &start, sizeof(uint16_t));
            check_write = write(fdpatch, &first, sizeof(uint8_t));
            check_write = write(fdpatch, &second, sizeof(uint8_t));
        }
        start++;
    }
}

int main(int argc, char* argv[]) {
    char* f1 = argv[1];
    char* f2 = argv[2];
    char* f3 = argv[3];

    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_RDONLY);
    int fd3 = open(f3, O_WRONLY | O_TRUNC);
    if(fd1 == -1 || fd2 == -1 || fd3 == -1) printf("Fd error."), exit(1);

    patch(fd1, fd2, fd3);

    close(fd1);
    close(fd2);
    close(fd3);
}
