#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int cut(int m, int n, char* filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return EXIT_FAILURE;
    }

    ssize_t chunk_sz = 1024, check_rd = 1;
    char in[chunk_sz];
    char out[chunk_sz];
    int pos = 0;

    while (check_rd)
    {
        check_rd = read(fd, in, chunk_sz);
        if (check_rd == -1)
        {
            perror("cut: read");
            close(fd);
            return EXIT_FAILURE;
        }

        ssize_t out_len = 0;
        for (ssize_t i = 0; i < check_rd; i++)
        {
            char c = in[i];
            if (c == '\n')
            {
                out[out_len++] = '\n';   /* end of line: emit newline, reset column */
                pos = 0;
            }
            else
            {
                pos++;
                if (pos >= m && pos <= n)
                    out[out_len++] = c;
            }
        }

        ssize_t written = 0;
        while (written < out_len)
        {
            ssize_t w = write(1, out + written, out_len - written);
            if (w == -1)
            {
                perror("cut: write");
                close(fd);
                return EXIT_FAILURE;
            }
            written += w;
        }
    }

    close(fd);
    return EXIT_SUCCESS;

}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./cut file\n");
        return EXIT_FAILURE;
    }

    int correct = 0, m = 0, n = 0;
    char m_bytes[100] = {'\0'};
    char n_bytes[100] = {'\0'};

    while (correct == 0)
    {
        ssize_t check_wr = write(1, "Enter m: ", 9);
        if (check_wr == -1)
        {
            perror("prompt m to stdin");
            return EXIT_FAILURE;
        }
        ssize_t check_rd = read(0, m_bytes, sizeof(m_bytes) - 1);
        if (check_rd == -1)
        {
            perror("read m from stdin");
            return EXIT_FAILURE;
        }
        m_bytes[check_rd] = '\0';

        check_wr = write(1, "Enter n: ", 9);
        if (check_wr == -1)
        {
            perror("prompt n to stdin");
            return EXIT_FAILURE;
        }
        check_rd = read(0, n_bytes, sizeof(n_bytes) - 1);
        if (check_rd == -1)
        {
            perror("read n from stdin");
            return EXIT_FAILURE;
        }
        n_bytes[check_rd] = '\0';

        m = atoi(m_bytes);
        n = atoi(n_bytes);

        if (m > n || m < 1 || n < 1)
        {
            fprintf(stderr, "m needs to be <= n\n");
            continue;
        }
        correct = 1;
    }

    return cut(m, n, argv[1]);
}