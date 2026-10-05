#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

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

int print_file_info(const char *name, const struct stat *info)
{
    char mode_text[11];
    const char *time_text;

    make_mode_string(info->st_mode, mode_text);
    time_text = ctime(&info->st_mtime);
    if (time_text == NULL)
    {
        fprintf(stderr, "%s: cannot format modification time\n", name);
        return 1;
    }

    printf("%s %3ju ", mode_text, (uintmax_t)info->st_nlink);
    printf("%-8s ", get_user_name(info->st_uid));
    printf("%-8s ", get_group_name(info->st_gid));
    printf("%8jd %.12s %s\n", (intmax_t)info->st_size,
           time_text + 4, name);
    return 0;
}

int list_files(const char *directory)
{
    DIR *stream;
    struct dirent *entry;
    struct stat info;
    int status = 0;

    stream = opendir(directory);
    if (stream == NULL)
    {
        perror(directory);
        return 1;
    }

    for (;;)
    {
        errno = 0;
        entry = readdir(stream);
        if (entry == NULL)
        {
            if (errno != 0)
            {
                perror(directory);
                status = 1;
            }
            break;
        }

        if (stat(entry->d_name, &info) == -1)
        {
            perror(entry->d_name);
            status = 1;
            continue;
        }

        if (print_file_info(entry->d_name, &info) != 0)
            status = 1;
    }

    if (closedir(stream) == -1)
    {
        perror(directory);
        status = 1;
    }

    return status;
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        return 1;
    }

    return list_files(argc == 2 ? argv[1] : ".");
}
