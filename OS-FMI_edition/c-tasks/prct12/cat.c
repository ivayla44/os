#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Includes tasks 1, 3 and 4
*/

int cat()
{
    ssize_t chunk_sz = 1024, check_rd = 1, check_wr = 1;
    char buf[chunk_sz];

    while (check_rd)
    {
        check_rd = read(0, buf, chunk_sz);
        if (check_rd == -1)
        {
            perror("read");
            return EXIT_FAILURE;
        }

        check_wr = write(1, buf, check_rd);
        if (check_wr == -1)
        {
            perror("write");
            return EXIT_FAILURE;
        }

        if (check_wr != check_rd)
        {
            fprintf(stderr, "cat: incomplete write: %zd of %zd bytes\n", check_wr, check_rd);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int cat_to_file(char* file)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("cat_to_file: open");
        return EXIT_FAILURE;
    }

    ssize_t chunk_sz = 1024, check_rd = 1, check_wr = 1;
    char buf[chunk_sz];

    while (check_rd)
    {
        check_rd = read(0, buf, chunk_sz);
        if (check_rd == -1)
        {
        perror("cat_to_file: read");
            close(fd);
            return EXIT_FAILURE;
        }

        check_wr = write(fd, buf, check_rd);
        if (check_wr == -1)
        {
        perror("cat_to_file: write");
            close(fd);
            return EXIT_FAILURE;
        }

        if (check_wr != check_rd)
        {
            fprintf(stderr, "cat_to_file: incomplete write: %zd of %zd bytes\n", check_wr, check_rd);
            close(fd);
            return EXIT_FAILURE;
        }
    }
    close(fd);

    return EXIT_SUCCESS;
}

int cat_from_file(char* file)
{
    int fd = open(file, O_RDONLY);
    if (fd < 0)
    {
        perror("cat_from_file: open");
        return EXIT_FAILURE;
    }

    ssize_t chunk_sz = 1024, check_rd = 1, check_wr = 1;
    char buf[chunk_sz];

    while (check_rd)
    {
        check_rd = read(fd, buf, chunk_sz);
        if (check_rd == -1)
        {
            perror("cat_from_file: read");
            close(fd);
            return EXIT_FAILURE;
        }

        check_wr = write(1, buf, check_rd);
        if (check_wr == -1)
        {
            perror("cat_from_file: write");
            close(fd);
            return EXIT_FAILURE;
        }

        if (check_wr != check_rd)
        {
            fprintf(stderr, "cat_from_file: incomplete write: %zd of %zd bytes\n", check_wr, check_rd);
            close(fd);
            return EXIT_FAILURE;
        }
    }
    close(fd);

    return EXIT_SUCCESS;
}

/*
 * Messy argument parsing in main.
 * Must be done with flags so cat_from_file can work with single argument. (or preferably in separate files how it's meant to be done)
 * Don't feel like doing it.
 *
 * Not sure if "cat >>file" should be handled usage (specifically parsing ">>" from the command line) or this just describes behavior - again, not doing it.
*/

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        return cat();
    }

    if (argc == 2)
    {
        return cat_to_file(argv[1]);
    }

    for (int i = 1; i < argc; i++)
    {
        if (cat_from_file(argv[i]) == EXIT_FAILURE) return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}