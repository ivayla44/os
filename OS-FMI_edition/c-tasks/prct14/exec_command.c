#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/* Да се напише програма на C, която получава като параметри в командния ред име (път) на команда (изпълним файл)
 * и списък с параметри за нея. Поражда процес наследник и сменя образа му с командата.
 * В родителя изчаква наследника да приключи и извежда номера му и кода на завършване.
*/

void exec_comm(char *comm_path, char** params) {
    pid_t p = fork();
    if (p < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (p == 0) {
        execv(comm_path, params);

        perror("exec error");
        exit(EXIT_FAILURE);
    }
    else {
        int status;
        if(wait(&status) == -1) {
            perror("wait error");
            exit(EXIT_FAILURE);
        }

        if(WIFEXITED(status)) {
            printf("command with path: \"%s\" executed in child process (PID: %d) | exited with code: %d\n", comm_path, p, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "command \"%s\" (PID %d) killed by signal %d\n", comm_path, p, WTERMSIG(status));
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ./out.o <path-to-command> [command params]\n");
        exit(EXIT_FAILURE);
    }
    exec_comm(argv[1], &argv[1]);

    return EXIT_SUCCESS;
}