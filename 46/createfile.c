#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("dat1", O_TRUNC | O_WRONLY);
    int fd2 = open("idx1", O_TRUNC | O_WRONLY);

    if(fd == -1 || fd2 == -1) {
        printf("Fd error.");
        return -1;
    }

    int check_write = 1;
    char* temp = "AbcdbcdBdgomf";
    check_write = write(fd, temp, 13);
    printf("%d\n", check_write);
    uint16_t temp16[] = {0, 4, 7, 10};
    uint8_t temp8[] = {4, 7, 3, 2, 3, 1, 3, 4};
    for(int i = 0, j = 0; i < 4; i++, j += 2) {
        check_write = write(fd2, &temp16[i], sizeof(uint16_t));
        check_write = write(fd2, &temp8[j], sizeof(uint8_t));
        check_write = write(fd2, &temp8[j+1], sizeof(uint8_t));
    }
    close(fd);
    close(fd2);
}