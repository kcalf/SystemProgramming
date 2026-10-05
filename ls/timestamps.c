#include <stdio.h>
#include <time.h>
#include <utime.h>

int main(void)
{
    struct utimbuf times;
    time_t now = time(NULL);

    if (now == (time_t)-1)
    {
        perror("time");
        return 1;
    }

    times.actime = now - 24 * 60 * 60;
    times.modtime = now - 48 * 60 * 60;

    if (utime("clock.txt", &times) == -1)
    {
        perror("clock.txt");
        return 1;
    }

    return 0;
}
