#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t cp(char* src, char* dst)
{
    int fd_src = open(src, O_RDONLY);
    if (fd_src < 0)
    {
        fprintf(stderr, "cp: cannot open source file: %s\n", src);
        return EXIT_FAILURE;
    }

    int fd_dst = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst < 0)
    {
        close(fd_src);
        fprintf(stderr, "cp: cannot open destination file: %s\n", dst);
        return EXIT_FAILURE;
    }

    ssize_t chunk_sz = 1024, check_rd = 1, check_wr = 1;
    char chunk[chunk_sz];

    while (1)
    {
        check_rd = read(fd_src, chunk, chunk_sz);
        if (check_rd == -1)
        {
            fprintf(stderr, "cp: cannot read from source file: %s\n", src);
            close(fd_src);
            close(fd_dst);
            return EXIT_FAILURE;
        }

        check_wr = write(fd_dst, chunk, check_rd);
        if (check_wr == -1)
        {
            fprintf(stderr, "cp: cannot write to destination file: %s\n", dst);
            close(fd_src);
            close(fd_dst);
            return EXIT_FAILURE;
        }

        if (check_rd != check_wr)
        {
            fprintf(stderr, "cp: incomplete write: %zd read from src - %zd written to dst\n" , check_rd, check_wr);
            close(fd_src);
            close(fd_dst);
            return EXIT_FAILURE;
        }

        if (check_rd == 0) break;
    }
    close(fd_src);
    close(fd_dst);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: cp <src> <dst>\n");
        return EXIT_FAILURE;
    }

    return cp(argv[1], argv[2]);
}
