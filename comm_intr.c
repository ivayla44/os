#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

char* readline(int fd) {
    int chunk = 16, check_read = 0, chunk_num = 0;
    char* line = malloc(chunk);
    char* read_buf = malloc(chunk);
    while(1) {
        check_read = read(fd, read_buf, chunk);
        if(check_read == -1) {
            free(line);
            free(read_buf);
            exit(1);
        }
        if(!check_read) {
            printf("wtf\n");
            break;
        }


        for(int i = 0; i < check_read; i++) {
            if(read_buf[i] != '\n') {
                line[chunk*chunk_num + i] = read_buf[i];
            }
            else {

                int pos = lseek(fd, i - check_read + 1, SEEK_CUR);
                printf("%d %d %d\n", i, chunk, pos);
                goto test;
            }
        }
        line = realloc(line, chunk*(chunk_num+2));
        chunk_num++;
    }
    test:
    return line;
}

int main(int argc, char* argv[]) {
    int fd = open(argv[1], O_RDONLY);
    char* line = readline(fd);
    printf("%s\n", line);
    char* line2 = readline(fd);
    printf("%s\n", line2);
    char* line3 = readline(fd);
    printf("%s\n", line3);
    char* line4 = readline(fd);
    printf("%s\n", line4);
    free(line);
}
