#include "error_handling.h"
#include <sys/wait.h>

int str_len(const char* str) {
    int i = 0;
    for(; str[i]; i++);
    return i;
}

// Just for testing.
int pipe_write(int write_pipe, char* str) {
    int str_sz = str_len(str);
    int check_write = err_write(write_pipe, str, str_sz);
    if(check_write == str_sz) return 1;
    if(check_write == 0) return 0;
    if(check_write == -1) {
        printf("Error in pipe write.\n");
        exit(1);
    }
    return 0;
}

// Just for testing.
char* pipe_chunk_read(int read_pipe) {
    int chunk_sz = 32;
    char* read_buf = malloc(chunk_sz);
    char* read = malloc(32);
    if(!read) {
        printf("Malloc error.");
        exit(2);
    }
    int temp_realloc = 1;
    while(1) {
        read = realloc(read, chunk_sz*temp_realloc);
        if(!read) {
            printf("Realloc error.");
            exit(3);
        }
        int check_read = err_read(read_pipe, read_buf, chunk_sz);
        if(check_read == -1) {
            free(read);
            exit(4);
        }
        for(int i = 0; i < check_read; i++) {
            read[i + (temp_realloc-1)*chunk_sz] = read_buf[i];
        }
        if(check_read < chunk_sz) break;
        temp_realloc++;
    }
    return read;
}

char** parseline(char* line) {
    int line_sz = str_len(line);

    char** parsed = malloc(sizeof(char*));
    if(!parsed) {
        printf("Malloc error.");
        return NULL;
    }
    for(int word_sz = 0, symbol = 0, word_num = 0; symbol <= line_sz; symbol++) {
        if(line[symbol] == ' ' || line[symbol] == '\0') {
            parsed[word_num] = malloc(word_sz + 1);
            parsed = realloc(parsed, sizeof(char*) * (word_num + 2));
            if(!parsed) {
                printf("Malloc error.");
                return NULL;
            }
            if(!parsed[word_num]) {
                for(int i = 0; i < word_num; i++) {
                    free(parsed[i]);
                }
                free(parsed);
                printf("Malloc error.");
                return NULL;
            }
            word_sz = 0;
            word_num++;
        }
        word_sz++;
    }
    int word = 0;
    for(int symbol_in_line = 0, symbol_in_parsed = 0; symbol_in_line <= line_sz; symbol_in_line++) {
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
    parsed[word + 1] = (char*)'\0';
    return parsed;
}

void close_pipe(int* pipe) {
    close(pipe[0]);
    close(pipe[1]);
}

void close_pipes(int** pipes){
    for(int i = 0; i < 5; i++) {
        if(pipes[i]) {
            close_pipe(pipes[i]);
        }
    }
}

void exec_command(char** command, int* stdin_pipe, int* stdout_pipe, int** pipes){
    int pid = fork();

    if(pid == 0) {
        int check_dup;

        if(stdin_pipe != NULL) {
            check_dup = dup2(stdin_pipe[0], 0);
            if(check_dup == -1) {
                printf("Dup error 1.\n");
                free(command);
                close_pipes(pipes);
                exit(5);
            }
        }

        if(stdout_pipe != NULL) {
            check_dup = dup2(stdout_pipe[1], 1);
            if(check_dup == -1) {
                printf("Dup error 2.\n");
                free(command);
                close_pipes(pipes);
                exit(6);
            }
        }

        close_pipes(pipes);

        int check_exec = execvp(command[0], command);
        if(check_exec == -1) {
            printf("Exec error.");
            free(command);
            exit(7);
        }
    }

    if(stdin_pipe) {
        close_pipe(stdin_pipe);
    }

    int check_wait = err_waitpid(pid, NULL, 0);
    if(check_wait == -1) exit(9);
}

int main() {
    char* commands[] = {"cut -f 7 -d : /etc/passwd", "sort", "uniq", "wc -l"};
    char** command_parsed;

    int child1_p[2];
    int check_pipe = pipe(child1_p);
    if(check_pipe == -1) {
        printf("Error with opening pipe 1.\n");
        exit(8);
    }

    int child2_p[2];
    check_pipe = pipe(child2_p);
    if(check_pipe == -1) {
        printf("Error with opening pipe 2.\n");
        close_pipe(child1_p);
        exit(8);
    }

    int child3_p[2];
    check_pipe = pipe(child3_p);
    if(check_pipe == -1) {
        printf("Error with opening pipe 3.\n");
        close_pipe(child1_p);
        close_pipe(child2_p);
        exit(8);
    }

    int* pipes[] = {NULL, child1_p, child2_p, child3_p, NULL};

    for(int curr_command = 0; curr_command < 4; curr_command++) {
        char* line = commands[curr_command];
        printf("command : %s\n", line);
        command_parsed = parseline(line);

        if(!command_parsed) {
            printf("Line couldn't be parsed.\n");
            close_pipes(pipes);
            exit(9);
        }

        exec_command(command_parsed, pipes[curr_command], pipes[curr_command + 1], pipes);

        for(int j = 0;; j++) {
            if(command_parsed[j]) {
                free(command_parsed[j]);
            }
            else {
                break;
            }
        }
        free(command_parsed);
    }

    close_pipes(pipes);
}
