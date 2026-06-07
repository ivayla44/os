#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int run_comm(char* comm) {
    pid_t p = fork();
    if (p < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (p == 0) {
        execlp(comm, comm, NULL);

        fprintf(stderr, "execlp command \"%s\" failed\n", comm);
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (wait(&status) == -1) {
            perror("wait failed");
            exit(EXIT_FAILURE);
        }

        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "command \"%s\" killed by signal %d\n", comm, WTERMSIG(status));
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_FAILURE;
}

/* 2)
 * Да се напише програма на C, която получава като параметри в командния ред 2 команди (без параметри).
 * Програмата реализира конструкцията: команда1 && команда2
 */

void comANDcom(char* comm1, char* comm2) {
    if (run_comm(comm1) != 0) {
        fprintf(stderr, "command 1 failed; not running command 2\n");
        exit(EXIT_FAILURE);
    }
    if (run_comm(comm2) != 0) {
        fprintf(stderr, "command 2 failed\n");
        exit(EXIT_FAILURE);
    }
    printf("both commands succeeded\n");
    exit(EXIT_SUCCESS);
}


/* 3)
 * Да се напише програма на C, която получава като параметри в командния ред 2 команди (без параметри).
 * Програмата реализира конструкцията: команда1 || команда2
 */

void comORcom(char* comm1, char* comm2) {
    if (run_comm(comm1) == 0) {
        printf("command 1 succeeded, not running command 2\n");
        exit(EXIT_SUCCESS);
    }
    if (run_comm(comm2) == 0) {
        printf("command 1 failed, but command 2 succeeded\n");
        exit(EXIT_SUCCESS);
    }
    printf("both commands failed\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./out.o <command1> <command2>\n");
        return EXIT_FAILURE;
    }

//    1)
    comANDcom(argv[1], argv[2]);

//    2)
//    comORcom(argv[1], argv[2]);
}