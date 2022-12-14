#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("44file", O_WRONLY);
    int fd2 = open("44file2", O_WRONLY);
    if(fd == -1 || fd2 == -1) {
        printf("Fd error.");
        return -1;
    }
    int check_write = 1;
    for(uint32_t i = 1; i <= 20; i++) {
        check_write = write(fd, &i, sizeof(uint32_t));
        printf("%d\n", check_write);
        if(check_write == -1) {
            printf("Write error.");
            return -1;
        }
    }
    for(uint32_t j = 3; j <= 6; j++) {
        check_write = write(fd2, &j, sizeof(uint32_t));
        printf("%d\n", check_write);
        if(check_write == -1) {
            printf("Write error.");
            return -1;
        }
    }
    close(fd);
    close(fd2);
    return 0;
}