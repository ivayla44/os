#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

 uint64_t to3ns(int num)
{
    uint64_t res = 0, temp;
    for(uint64_t i = 1; num > 0; i *= 10)
    {
        temp = num % 3;
        num /= 3;
        res += temp * i;
    }
    return res;
}

void all3ns(int num){
    for(int i = 0; i < num; i++) {
        printf("%llu\n", to3ns(i));
    }
}

void tnsFile(int fd, int num) {
    int check_write = 1;
    uint64_t rem;
    for(int i = 0; i < num; i++) {
        uint64_t res = to3ns(i);
        char itos[12] = {'0', '0', '0', '0', '0', '0', '0' ,'0', '0', '0', '0', '0'};
        for(int j = 11; res > 0; j--) {
            rem = res % 10;
            res /= 10;
            itos[j] = rem + '0';
        }
        check_write = write(fd, itos, 12);
        check_write = write(fd, "\n", 1);
    }
}

int main() {
//    int fd = open("file", O_WRONLY | O_TRUNC);
    int fd = open("file", O_WRONLY | O_TRUNC);
    if(fd == -1){
        printf("Fd error.");
        return -1;
    }
    all3ns(17);
//    tnsFile(fd, 19683); // 3^9
    tnsFile(fd, 531441); // 3^12
    close(fd);
}
