#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int _strlen(char* txt)
{
    int len = 0;
    while (txt[len] != '\0') len++;
    return len;
}

int fgrep(char *text, char* filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    int text_len = _strlen(text);
    ssize_t chunk_sz = 81, check_rd = 1;
    char chunk[chunk_sz];

    while (check_rd)
    {
        ssize_t len = 0;
        while (len < chunk_sz - 1)
        {
            char c;
            check_rd = read(fd, &c, 1);
            if (check_rd == -1)
            {
                close(fd);
                perror("read");
                return -1;
            }
            if (check_rd == 0) break;
            if (c == '\n') break;
            chunk[len++] = c;
        }
        chunk[len] = '\0';

        for (ssize_t chunk_i = 0; chunk_i < len; chunk_i++)
        {
            ssize_t txt_i = 0;
            while (txt_i < text_len && chunk[chunk_i + txt_i] == text[txt_i])
                txt_i++;
            if (txt_i == text_len)
            {
                close(fd);
                return 0;
            }
        }
    }

    close(fd);
    return -1;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <text to search for> <pathname>\n", argv[0]);
    }

    if (fgrep(argv[1], argv[2]) != 0) fprintf(stdout, "fgrep: text: \"%s\" not found in file: %s\n", argv[1], argv[2]);
    else fprintf(stdout, "fgrep - text: \"%s\" found in file: %s\n", argv[1], argv[2]);

    return EXIT_SUCCESS;
}