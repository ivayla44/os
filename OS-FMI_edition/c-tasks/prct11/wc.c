#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdbool.h>

bool is_whitespace(const char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

/*
 * Print newline, word, and byte counts for FILE.
 * A word is a non-zero-length sequence of printable characters delimited by white space.
 */

uint8_t wc(char* file)
{
    int fd = open(file, O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "wc: cannot open %s\n", file);
        return EXIT_FAILURE;
    }

    uint32_t lines = 0, words = 0, bytes = 0;
    bool in_word = false;

    ssize_t chunk_sz = 1024, check_rd = 1;
    char chunk[chunk_sz];

    while (1)
    {
        check_rd = read(fd, chunk, chunk_sz);
        if (check_rd == -1)
        {
            fprintf(stderr, "wc: cannot read %s\n", file);
            close(fd);
            return EXIT_FAILURE;
        }

        ssize_t i = 0;
        for (; i < check_rd; i++)
        {
            if (chunk[i] == '\n') lines++;
            if (is_whitespace(chunk[i])) in_word = false;
            else if (!in_word)
            {
                in_word = true;
                words++;
            }
        }
        bytes += i;

        if (!check_rd) break;
    }
    close(fd);

    fprintf(stdout, "Lines: %d; Words: %d; Bytes: %d\n", lines, words, bytes);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: wc filename\n");
        return EXIT_FAILURE;
    }

    return wc(argv[1]);
}