#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

void make_mode_string(mode_t mode, char result[])
{
    const mode_t masks[] = {
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
    };
    const char letters[] = "rwxrwxrwx";
    int index;

    result[0] = '-';
    if (S_ISDIR(mode))
        result[0] = 'd';
    else if (S_ISCHR(mode))
        result[0] = 'c';
    else if (S_ISBLK(mode))
        result[0] = 'b';

    for (index = 0; index < 9; index++)
        result[index + 1] = (mode & masks[index]) ? letters[index] : '-';

    result[10] = '\0';
}

int main(int argc, char *argv[])
{
    struct stat info;
    char mode_text[11];

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

    make_mode_string(info.st_mode, mode_text);
    printf("%s %s\n", mode_text, argv[1]);
    return 0;
}
