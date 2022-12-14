#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int to3ns(int num) {
    int res = 0, temp;
    for(int i = 1; num > 0; i *= 10) {
        temp = num % 3;
        num /= 3;
        res += temp * i;
    }
    return res;
}

void all3ns(int num) {
    for(int i = 0; i < num; i++) {
        printf("%d\n", to3ns(i));
    }
}

void tnsFile(int fd, int num) {
    int check_write = 1;
    int rem;
    for(int i = 0; i < num; i++) {
        int res = to3ns(i);
        char itos[9] = {'0', '0', '0', '0', '0', '0', '0' ,'0', '0'};
        for(int j = 8; res > 0; j--) {
            rem = res % 10;
            res /= 10;
            itos[j] = rem + '0';
        }
        check_write = write(fd, itos, 9);
        check_write = write(fd, "\n", 1);
    }
}

int main() {
    int fd = open("file", O_WRONLY | O_TRUNC);
    if(fd == -1) {
        printf("Fd error.");
        return -1;
    }
    all3ns(17);
    tnsFile(fd, 19683); // 3^9
    close(fd);
}
