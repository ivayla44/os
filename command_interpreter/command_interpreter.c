#include "error_handling.h"
#include <sys/wait.h>

int str_len(const char* str) {
    int i = 0;
// kpacu: '\0' != str[i]
// kpacu: I'd write it like ^ that.
    for(; str[i]; i++);
    return i;
}

int get_lines(int fd) {
    int ch_sz = 32, check_read = 1, res = 0;
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
// kpacu: realloc regardless whether check_read == 0?
// Seems like a waste.
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
    // the idea is to malloc everything first and then fill in the symbols
    // kpacu: The "for" and its <body> are tough to read through for me.
    // Took me a few attempts. Can it be refactored?
    for(int word_sz = 0, symbol = 0, word_num = 0; symbol <= line_sz; symbol++) {
	// kpacu: if the end of a word is found
        if(line[symbol] == ' ' || line[symbol] == '\0') {
            // malloc new sizeof(char*) each time and then malloc the appropriate amount for each word (+ 1 for null)
            // kpacu: Alloc memory for the word and the new word_pointer.
            parsed[word_num] = malloc(word_sz + 1);
            parsed = realloc(parsed, sizeof(char*) * (word_num + 2));
            // kpacu: error handling
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
            // kpacu: Reset word_length counter, increase words_found counter.
            // kpacu: I've no idea why I put that there. Think it took me a while to
            // figure out what/why/how this "for" was doing and I needed a marker.
            word_sz = 0; // :/ - i'm not doing <word_sz = -1> anymore :)
            word_num++;
        }
	// kpacu: if not delimiter, increase word_length counter.
        word_sz++;
    }
    int word = 0;
    for(int symbol_in_line = 0, symbol_in_parsed = 0; symbol_in_line <= line_sz; symbol_in_line++) {
        // kpacu: If the current symbol of the line is not a seperator ...
        if(line[symbol_in_line] != ' ') {
            // kpacu: ...  move the symbol to the token it's part of.
            parsed[word][symbol_in_parsed] = line[symbol_in_line];
            symbol_in_parsed++;
        }
        // kpacu: If it's a token separator ...
        else { 
            // kpacu: ... '\0' terminate the current token ...
            parsed[word][symbol_in_parsed] = '\0';
            // kpacu: ... reset token_symbols counter ...
            symbol_in_parsed = 0;
            // kpacu: ... increase the current_token counter.
            word++;
        }
    }
    // kpacu: NULL terminate the strings array.
    // kpacu: The cast is interesting. Why:
    // (char*)'\0'
    // instead of:
    // NULL
    parsed[word + 1] = (char*)'\0';
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

// kpacu: This snipped is commonly written by students.
// Could be moved to a reset_fd() function? Two reasons:
// 1) In future situations we might need to reset two (or more) file descriptors.
// 2) The name reset_fd() is pretty self explanatory (there might be an even better
// function name). Thus, the reader does not need to parse all the code, just the name.
    int check_lseek = err_lseek(fd, 0, SEEK_SET);
    if(check_lseek == -1) {
        exit(4);
    }

    char** parsed;

    int pid, status, check_wait, check_exec;

    for(int i = 0; i < num_lines; i++) {
        pid = fork();
        if(pid == 0) {
            line = readline(fd);
            parsed = parseline(line);
            printf("%s\n", parsed[0]);

            check_exec = execvp(parsed[0], parsed);
            if(check_exec == -1) {
                printf("Exec error.");
                exit(12);
            }
        }
// kpacu: Ok, so the parent executes the code in the "else" block, but it also
// executes the code after the whole "if then else" statement. The question is,
// why split the code into two scopes? The instructions will always be executed
// in the following order: q1 => q2 => p1 => p2 
        else {
            // q1
            check_wait = wait(&status);
            // q2
            if(check_wait == -1) {
                printf("Wait error.");
                exit(11);
            }
        }
        // p1
        if(WIFEXITED(status) == 1) {
            printf("Exit status: %d\n", WEXITSTATUS(status));
        }
        // kpacu: "if (! WIFEXITED(status))" will be true only "if( WIFEXITED(status)) == 0".
        // Can WIFEXITSTATUS == anything besides a 1 or a 0? The "if" after the "else" seems
        // a bit sus.
        // p2
        else if(!WIFEXITED(status)){
            printf("Process terminated.\n");
            exit(10);
        }
    }
}
