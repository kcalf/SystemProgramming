#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    mode_t runner_mode = S_IRWXU | S_IRGRP | S_IXGRP;

    if (chmod("private.txt", 0640) == -1)
    {
        perror("private.txt");
        return 1;
    }

    if (chmod("runner.txt", runner_mode) == -1)
    {
        perror("runner.txt");
        return 1;
    }

    return 0;
}
