#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    const char *path = "sample/data.txt";
    struct stat info;

    if (stat(path, &info) == -1)
    {
        perror(path);
        return 1;
    }

    printf("%s: %jd bytes\n", path, (intmax_t)info.st_size);
    return 0;
}
