#include "error_handling.h"
#include <sys/wait.h>

int subst_bytes(int input_fd, int output_fd) {
    int check_read, check_write;
    int chunk_sz = 32;
    unsigned char buf;
    unsigned char chunk[chunk_sz];
    while(1) {
        check_read = err_read(input_fd, chunk, chunk_sz);
        if(check_read == -1) {
            return -1;
        }
        if(check_read == 0) {
            break;
        }

        for(int read = 0; read < chunk_sz; read++) {
            if(chunk[read] == 0x55) {
                continue;
            }
            else if(chunk[read] == 0x7D && read+1 != chunk_sz) {
                buf = chunk[read+1] ^ 0x20;
                read++;
            }
            else {
                buf = chunk[read];
            }
            check_write = err_write(output_fd, &buf, 1);
            if(check_write == -1) {
                return -1;
            }
        }
    }
    return 0;
}

void close_pipe(int* pipe) {
    if(pipe) {
        close(pipe[0]);
        close(pipe[1]);
    }
}

int main(int argc, char* argv[]) {
    int check_pipe;
    int p[2];
    check_pipe = pipe(p);
    if(check_pipe == -1) {
        printf("Pipe error.\n");
        exit(1);
    }

    int pid = fork();

    if(pid == 0) {
        int check_dup, check_exec;

        check_dup = dup2(p[1], 1);
        if(check_dup == -1) {
            printf("Dup error.\n");
            close_pipe(p);
            exit(2);
        }

        close(p[1]);

        check_exec = execlp("cat", argv[1], (char*) NULL);
        if(check_exec == -1) {
            printf("Exec error.\n");
            exit(3);
        }
    }

    int fd_out, check_subst;

    fd_out = err_open(argv[2], O_WRONLY);
    if(fd_out == -1) {
        close(p[0]);
        exit(4);
    }
    check_subst = subst_bytes(p[0], fd_out);

    if(check_subst == -1) {
        close(p[0]);
        close(fd_out);
        exit(5);
    }
}