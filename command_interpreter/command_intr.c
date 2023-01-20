#include "error_handling.h"

int str_len(const char* str) {
    int i = 0;
    for(; str[i]; i++);
    return i;
}

int get_lines(int fd) {
    int ch_sz = 32, check_read = 1, res = 1;
    char chunk[ch_sz];
    while (1) {
        check_read = err_read(fd, &chunk, ch_sz);
        if (!check_read) break;
        if (check_read == -1) {
            exit(7);
        }
        for (int i = 0; i < check_read; i++) {
            if (chunk[i] == '\n') res++;
        }
    }
    return res;
}

char* readline(int fd) {
    int chunk_sz = 16, check_read = 1, check_lseek, chunk_num = 0;
    char* line = malloc(chunk_sz);
    if(!line) {
        printf("Malloc error.");
        exit(1);
    }
    char* read_buf = malloc(chunk_sz);
    if(!read_buf) {
        printf("Malloc error.");
        free(line);
        exit(2);
    }
    while(check_read) {
        check_read = err_read(fd, read_buf, chunk_sz);
        if(check_read == -1) {
            free(line);
            free(read_buf);
            exit(3);
        }
        if(!check_read) {
            break;
        }

        for(int i = 0; i < check_read; i++) {
            if(read_buf[i] != '\n') {
                line[chunk_sz*chunk_num + i] = read_buf[i];
            }
            else {
                check_lseek = err_lseek(fd, i - check_read + 1, SEEK_CUR);
                if(check_lseek == -1) {
                    free(line);
                    free(read_buf);
                    exit(4);
                }
                check_read = 0;
                break;
            }
        }
        line = realloc(line, chunk_sz*(chunk_num+2));
        if(!line) {
            printf("Realloc error.");
            free(read_buf);
            exit(5);
        }
        chunk_num++;
    }
    free(read_buf);
    int line_sz = str_len(line);
    line[line_sz] = '\0';
    return line;
}

char** parseline(char* line) {
    int line_sz = str_len(line);
    char** parsed = malloc(sizeof(char*));
    if(!parsed) {
        free(line);
        printf("Malloc error.");
        exit(6);
    }
    for(int word_sz = 0, symbol = 0, word_num = 0; symbol <= line_sz; symbol++) {
        if(line[symbol] == ' ' || line[symbol] == '\0') {
            parsed[word_num] = malloc(word_sz + 1);
            parsed = realloc(parsed, sizeof(char*) * (word_num + 2));
            if(!parsed) {
                free(line);
                printf("Malloc error.");
                exit(6);
            }
            if(!parsed[word_num]) {
                for(int i = 0; i < word_num; i++) {
                    free(parsed[i]);
                }
                free(parsed);
                free(line);
                printf("Malloc error.");
                exit(6);
            }
            word_sz = 0; // :/
            word_num++;
        }
        word_sz++;
    }
    for(int word = 0, symbol_in_line = 0, symbol_in_parsed = 0; symbol_in_line <= line_sz; symbol_in_line++) {
        if(line[symbol_in_line] != ' ') {
             parsed[word][symbol_in_parsed] = line[symbol_in_line];
             symbol_in_parsed++;
        }
        else {
            parsed[word][symbol_in_parsed] = '\0';
            symbol_in_parsed = 0;
            word++;
        }
    }
    free(line);
    return parsed;
}

int main(int argc, char* argv[]) {
    int fd = err_open(argv[1], O_RDONLY);
    if(fd == -1) {
        exit(8);
    }

    int num_lines = get_lines(fd);

    char* line;

    int check_lseek = err_lseek(fd, 0, SEEK_SET);
    if(check_lseek == -1) {
        exit(4);
    }

    char** parsed;

    int pid, status, check_wait;

    for(int i = 0; i < num_lines; i++) {
        pid = fork();
        if(pid == 0) {
            line = readline(fd);
            parsed = parseline(line);
            printf("%s\n", parsed[0]);

            execvp(parsed[0], parsed);

            break;
        }
        else {
           check_wait = wait(&status);
           if(check_wait == -1) {
               printf("Wait error.");
               exit(11);
           }
        }
        if(WIFEXITED(status) == 1) {
            printf("Exit status: %d\n", WEXITSTATUS(status));
        }
        else if(!WIFEXITED(status)){
            printf("Process terminated.\n");
            exit(10);
        }
    }
}
