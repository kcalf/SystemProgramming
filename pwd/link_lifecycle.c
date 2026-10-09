#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<inttypes.h>

static int show_metadata(const char *stage, const char *path)
{
  struct stat info;

  if(stat(path, &info) == -1){
    perror(path);
    return -1;
  }
  printf("%s: %s inode=%ju links=%ju\n", stage, path, (uintmax_t)info.st_ino, (uintmax_t)info.st_nlink);

  return 0;
}

int main()
{
  const char *original = "fs_lab/documents/report.txt";
  const char *alias = "fs_lab/archive/report_link.txt";
  int failed = 0;

  if(show_metadata("before", original) == -1)
  {
    return 1;
  }

  if(link(original, alias) == -1)
  {
    perror("link");
    return 1;
  }

  if(show_metadata("linked", original) == -1 || show_metadata("linked", alias) == -1)
  {
    failed = 1;
  }
  if(unlink(alias) == -1)
  {
    perror("unlink");
    return 1;
  }

  if(failed || show_metadata("unlinked", original) == -1)
  {
    return 1;
  }

  return 0;
}
