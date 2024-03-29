#include "error_handling.h"
#include <sys/wait.h>

int subst_bytes(int input_fd, int output_fd) {
    int check_read, check_write;

    int chunk_sz = 32;
    // Unsigned za da moje vsichki stoinost
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

        for(int read = 0; read < check_read; read++) {
            // Propuska 55
            if(chunk[read] == 0x55) {
                continue;
            }
            // Propuska 7d i xor-va sledvashtoto
            // Ako 7d e posledno go priema za 7d
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

        close_pipe(p);

        // Poradi nqkakva bezizvestna prichina <execlp> ne raboti. Bukvalno ne znam zashto ama trqbva da razbera che mi e tupo :(
	// kpacu: pitai :)
        // Uj go vikam kakto trqbva ama neshto si pravi topki s men (rechnik)
        // To i git si pravi qko tashaci
	// kpacu: rechnik? :)
        // Sigurno az sum prosta i za dvete ama zasega obvinqvam vselenata, az nikoga ne sum vinovna (veroqtno sum)
	// kpacu: Ne si prosta, spodelqm mnenieto ti za vselenata.

	// marker start
        // char* command[3] = {"cat", argv[1], NULL};
        // check_exec = execvp(command[0], command);
	// marker end
	// marker start
	check_exec = execlp("cat", "cat", argv[1], NULL);
	//marker end
        if(check_exec == -1) {
            printf("Exec error.\n");
            exit(3);
        }
    }

    int check_wait;

    check_wait = waitpid(pid, NULL, 0);
    if(check_wait == -1) {
        printf("Wait error.\n");
        exit(6);
    }

    int fd_out, check_subst;

    fd_out = err_open(argv[2], O_WRONLY);
    if(fd_out == -1) {
        close(p[0]);
        exit(4);
    }

    close(p[1]);

    check_subst = subst_bytes(p[0], fd_out);

    if(check_subst == -1) {
        close(p[0]);
        close(fd_out);
        exit(5);
    }
    
    close_pipe(p);
    close(fd_out);

    return 0;
}
