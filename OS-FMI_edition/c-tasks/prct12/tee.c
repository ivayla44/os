#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int tee(char *file)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("file open");
        return EXIT_FAILURE;
    }

    ssize_t chunk_sz = 1024, check_rd = 1, check_wr = 1;
    char buf[chunk_sz];

    while (check_rd)
    {
        check_rd = read(0, buf, chunk_sz);
        if (check_rd == -1)
        {
            perror("read");
            close(fd);
            return EXIT_FAILURE;
        }

        check_wr = write(1, buf, check_rd);
        if (check_wr == -1)
        {
            perror("write to stdout");
            close(fd);
            return EXIT_FAILURE;
        }

        if (check_wr != check_rd)
        {
            fprintf(stderr, "tee: incomplete write to stdout: %zd of %zd bytes\n", check_wr, check_rd);
            close(fd);
            return EXIT_FAILURE;
        }

        check_wr = write(fd, buf, check_rd);
        if (check_wr == -1)
        {
            perror("write to file");
            close(fd);
            return EXIT_FAILURE;
        }

        if (check_wr != check_rd)
        {
            fprintf(stderr, "tee: incomplete write to file: %zd of %zd bytes\n", check_wr, check_rd);
            close(fd);
            return EXIT_FAILURE;
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: tee <file>\n");
        return EXIT_FAILURE;
    }

    return tee(argv[1]);
}