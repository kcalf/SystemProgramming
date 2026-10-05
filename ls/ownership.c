#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    uid_t owner = 1001;
    gid_t group = 1001;

    if (chown("owner.txt", owner, group) == -1)
    {
        perror("owner.txt");
        return 1;
    }

    return 0;
}
