#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void print_ps_par_pids()
{
    printf("Process PID: %d | Parent PID: %d\n", getpid(), getppid());
    fflush(stdout);
}

/* 1)
 * Да се напише програма на C, която извежда номера на процеса и номера на родителя му.
 * Сменя образа му с UNIX команда (например ps) и извежда резултата от exec().
*/
void pids_exec()
{
    print_ps_par_pids();

    execlp("ps", "ps", NULL);

    perror("execlp");
    exit(EXIT_FAILURE);
}

/* 2)
 * Да се напише програма на C, която извежда номера на процеса и номера на родителя му.
 * Поражда процес наследник.
 * Във всеки от процесите извежда резултата от fork(), номера на процеса и номера на родителя му.
*/

void fork_pids()
{
    print_ps_par_pids();

    pid_t p = fork();
    if (p == -1)
    {
        perror("fork error: return value -1");
        exit(EXIT_FAILURE);
    }

    if (p == 0)
    {
        printf("[CHILD] fork returned: %d | Process PID: %d | Parent PID: %d\n", p, getpid(), getppid());
        exit(EXIT_SUCCESS);
    }
    else
    {
        printf("[PARENT] fork returned: %d | Process PID: %d | Parent PID: %d\n", p, getpid(), getppid());
        wait(NULL);
    }
}

/* 3)
 *  Да се напише програма на C, която извежда номера на процеса и номера на родителя му.
 *  Поражда процес наследник.
 *  Във всеки от процесите извежда резултата от fork(), номера на процеса и номера на родителя му.
 *  В родителя изчаква наследника да приключи и извежда номера му и кода на завършване.
*/

void fork_pids_exit_st()
{
    print_ps_par_pids();

    pid_t p = fork();
    if (p == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (p == 0)
    {
        printf("[CHILD] fork return value: %d | process PID: %d, parent PID: %d\n", p, getpid(), getppid());
        exit(42);
    }
    else
    {
        printf("[PARENT] fork return value: %d | process PID: %d, parent PID: %d\n", p, getpid(), getppid());

        int status;
        if (wait(&status) == -1)
        {
            perror("wait error");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            printf("child process exited. pid: %d | exit code: %d\n", p, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status))
        {
            fprintf(stderr, "child killed by signal %d\n", WTERMSIG(status));
        }
    }
}

/* 4)
 * Да се напише програма на C, която извежда номера на процеса и номера на родителя му.
 * Поражда процес наследник.
 * Във всеки от процесите извежда резултата от fork(), номера на процеса и номера на родителя му.
 * Сменя образа на наследника с UNIX команда.
 * В родителя изчаква наследника да приключи и извежда номера му и кода на завършване.
*/

void pids_fork_exec_exts()
{
    print_ps_par_pids();

    pid_t p = fork();
    if (p == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (p == 0)
    {
        printf("[CHILD] fork return value: %d | process PID: %d, parent PID: %d\n", p, getpid(), getppid());
        fflush(stdout);

        execlp("uname", "uname", "-r", NULL);

        perror("strace error");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[PARENT] fork return value: %d | process PID: %d, parent PID: %d\n", p, getpid(), getppid());

        int status;
        if (wait(&status) == -1)
        {
            perror("wait error");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status))
        {
            printf("child process PID: %d, exit status: %d\n", p, WEXITSTATUS(status));
        }
    }
}

int main(int argc, char* argv[])
{
    // printf("1) \n");
    // pids_exec();

    // printf("2) \n");
    // fork_pids();

    // printf("3) \n");
    // fork_pids_exit_st();

    printf("4) \n");
    pids_fork_exec_exts();
}