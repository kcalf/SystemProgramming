#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static int inode_of(const char *path, struct stat *info);
static int find_entry_name(ino_t target, char *name, size_t size);
static int print_absolute_path(int *has_component);

static int inode_of(const char *path, struct stat *info)
{
    return stat(path, info);
}

static int find_entry_name(ino_t target, char *name, size_t size)
{
    DIR *stream = opendir(".");
    if (stream == NULL)
        return -1;

    int result = -1;
    int saved_error = ENOENT;

    for (;;) {
        errno = 0;
        struct dirent *entry = readdir(stream);
        if (entry == NULL) {
            if (errno != 0)
                saved_error = errno;
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        if (entry->d_ino != target)
            continue;

        size_t length = strlen(entry->d_name);
        if (length >= size) {
            saved_error = ENAMETOOLONG;
            break;
        }
        memcpy(name, entry->d_name, length + 1);
        result = 0;
        break;
    }

    if (closedir(stream) == -1 && result == 0)
        return -1;
    if (result == -1)
        errno = saved_error;
    return result;
}

static int print_absolute_path(int *has_component)
{
    struct stat current, parent;
    char name[256];

    if (inode_of(".", &current) == -1 ||
        inode_of("..", &parent) == -1)
        return -1;

    if (current.st_dev == parent.st_dev &&
        current.st_ino == parent.st_ino)
        return 0;

    if (current.st_dev != parent.st_dev) {
        errno = EXDEV;
        return -1;
    }
    if (chdir("..") == -1 ||
        find_entry_name(current.st_ino, name, sizeof name) == -1)
        return -1;

    if (print_absolute_path(has_component) == -1)
        return -1;
    if (printf("/%s", name) < 0)
        return -1;
    *has_component = 1;
    return 0;
}

int main(void)
{
    int has_component = 0;

    if (print_absolute_path(&has_component) == -1) {
        perror("my_pwd");
        return 1;
    }
    if (!has_component && putchar('/') == EOF) {
        perror("stdout");
        return 1;
    }
    if (putchar('\n') == EOF || fflush(stdout) == EOF) {
        perror("stdout");
        return 1;
    }
    return 0;
}
