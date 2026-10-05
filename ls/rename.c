#include <stdio.h>

int main(void)
{
    if (rename("report.txt", "report_old.txt") == -1)
    {
        perror("rename");
        return 1;
    }

    return 0;
}
