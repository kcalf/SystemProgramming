#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    struct stat info;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    if (stat(argv[1], &info) == -1)
    {
        perror(argv[1]);
        return 1;
    }

    printf("mode:  %jo\nlinks: %ju\nuid:   %ju\ngid:   %ju\n",
           (uintmax_t)info.st_mode, (uintmax_t)info.st_nlink,
           (uintmax_t)info.st_uid, (uintmax_t)info.st_gid);
    printf("size:  %jd\nmtime: %jd\nname:  %s\n",
           (intmax_t)info.st_size, (intmax_t)info.st_mtime, argv[1]);

    return 0;
}
