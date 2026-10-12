#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>

static int send_line(int target, const char *line){
  size_t length = strlen(line); // 다른 터미널로 보낼 총 문자열 길이
  size_t sent = 0; // 현재까지 실제로 보낸 바이트 수

  // 요청한 길이를 모두 보낼 때까지 반복
  while(sent < length){
    ssize_t count = write(target, line + sent, length - sent);

    if(count == -1){
      if(errno == EINTR){ // 시그널 방해로 중단된 경우, while문 재시도
        continue;
      }
      perror("write");
      return -1;
    }
    if(count == 0){
      fprintf(stderr, "write: no progress\n");
      return -1;
    }
    sent += (size_t)count;
  }

  return 0;
}

int main(int argc, char *argv[]){
  char line[BUFSIZ]; // 한 줄을 읽어올 버퍼
  int result = 0;

  if(argc != 2){
    fprintf(stderr, "Usage: %s /dev/pts/N\n", argv[0]);
    return 1;
  }

  int target = open(argv[1], O_WRONLY); // 대상 터미널 열기: 인자로 받은 경로를 쓰기 전용으로 엶
  if(target == -1){
    perror(argv[1]);
    return 1;
  }

  while(fgets(line, sizeof line, stdin) != NULL){
    if(send_line(target, line) == -1){
      result = 1;
      break;
    }

    if(ferror(stdin)){
      perror("fgets");
      result = 1;
    }
  }
  if(close(target) == -1){
    perror("close");
    result = 1;
  }
  
  return result;
}
