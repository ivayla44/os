#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("f1", O_TRUNC | O_WRONLY);
    int fd2 = open("f2", O_TRUNC | O_WRONLY);

    if(fd == -1 || fd2 == -1) {
        printf("Fd error.");
        return -1;
    }

    int check_write = 1;
    char* temp = "Abcdefghijklm", *temp2 = "Abcdffghijjjj";
    check_write = write(fd, temp, 13);
    printf("%d\n", check_write);
    check_write = write(fd2, temp2, 13);
    printf("%d\n", check_write);

    close(fd);
    close(fd2);
}