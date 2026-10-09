#include<stdio.h>
#include<sys/stat.h>
#include<inttypes.h>
#include<errno.h>

int main()
{
  const char *old_path = "fs_lab/documents/report.txt";
  const char *new_path = "fs_lab/archive/report_final.txt";
  struct stat before, after, probe;

  if(stat(old_path, &before) == -1)
  {
    perror(old_path);
    return 0;
  }

  if(rename(old_path, new_path) == -1)
  {
    perror("rename");
    return 1;
  }

  if(stat(new_path, &after) == -1)
  {
    perror(new_path);
    return 1;
  }

  printf("before inode=%ju; after inode=%ju\n", (uintmax_t)before.st_ino, (uintmax_t)after.st_ino);

  if (before.st_dev != after.st_dev || before.st_ino != after.st_ino)
  {
      fputs("File identity changed.\n", stderr);
      return 1;
  }
  if(stat(old_path, &probe) == 0)
  {
    fputs("Old path still exists. \n", stderr);
    return 1;
  }
  if(errno != ENOENT)
  {
    perror(old_path);
    return 1;
  }

  puts("Old path removed; new path refers to the same inode.");

  return 0;
}
