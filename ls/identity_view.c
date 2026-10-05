#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

const char *get_user_name(uid_t uid)
{
    struct passwd *account = getpwuid(uid);
    static char number[3 * sizeof(uintmax_t) + 1];

    if (account != NULL)
        return account->pw_name;

    snprintf(number, sizeof(number), "%ju", (uintmax_t)uid);
    return number;
}

const char *get_group_name(gid_t gid)
{
    struct group *group = getgrgid(gid);
    static char number[3 * sizeof(uintmax_t) + 1];

    if (group != NULL)
        return group->gr_name;

    snprintf(number, sizeof(number), "%ju", (uintmax_t)gid);
    return number;
}

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

    printf("owner: %s (UID %ju)\n", get_user_name(info.st_uid),
           (uintmax_t)info.st_uid);
    printf("group: %s (GID %ju)\n", get_group_name(info.st_gid),
           (uintmax_t)info.st_gid);
    return 0;
}
