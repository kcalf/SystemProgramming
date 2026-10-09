#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>

static int make_directory(const char *path)
{
  if(mkdir(path, 0755) == -1)
  {
    perror(path);
    return -1;
  }
}

static int move_to(const char *path)
{
  if(chdir(path) == -1){
    perror(path);
    return -1;
  }
}

int main()
{
   if (make_directory("fs_lab") == -1 ||
        move_to("fs_lab") == -1 ||
        make_directory("documents") == -1 ||
        make_directory("archive") == -1 ||
        make_directory("workspace") == -1 ||
        move_to("workspace") == -1 ||
        make_directory("module") == -1 ||
        make_directory("scratch") == -1)
        return 1;

  if(rmdir("scratch") == -1)
  {
    perror("rmdir scratch");
    return 1;
  }

  if(move_to("..") == -1)
  {
    return 1;
  }

  puts("Created fs_lab; removed workspace/scratch.");
  
  return 0;
}
