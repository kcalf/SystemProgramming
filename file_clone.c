/*
  4096 Byte 단위로 읽어 Buffer에 담고, 읽어 온 만큼 destination 파일에 기록하는 간단한 cp 구현
  main(): 명령행 인자를 검사하고 복사 요청
  copy_file(): Source를 읽고 Destination에 쓰는 정체 복사 과정
  write_all(): write()가 일부만 기록했을 경우, 남은 데이터까지 모두 기록 
*/
#define BUFFER_SIZE 4096

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h> // 파일 권한과 관련된 타입 및 정의

static int write_all(int fd, const char *buffer, ssize_t length){
  // Buffer에 들어 있는 length Byte를 fd가 가리키는 파일에 모두 기록
  // fd: 데이터를 기록할 Destination의 File Descriptor
  // buffer: 기록할 데이터가 들어 있는 Buffer
  // length: 실제로 기록해야 할 Byte 수
  
  ssize_t written = 0; // 현재까지 몇 Byte를 기록했는지 저장

  while(written < length){
    ssize_t result = write(fd, buffer + written, (size_t)(length-written));
    // buffer + written: 아직 기록하지 않은 데이터의 시작 위치
    // length - written: 앞으로 기록해야 할 남은 Byte 수

    if(result == -1){
      return -1;
    }

    written += result;
  }

  return 0;
}

static int copy_file(const char *source_path, const char *dest_path){
  // source_path: 복사할 원본 파일
  // dest_path: 새로 만들거나 덮어쓸 목적 파일
  
  int source_fd = open(source_path, O_RDONLY);
  if(source_fd == -1){
    perror("open source");
    return -1;
  }

  int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  // O_WRONLY: 쓰기 전용
  // O_CREAT: 파일이 없으면 생성
  // O_TRUNC: 기존 파일이면 내용을 지우고 크기를 0으로 만듦 
  if(dest_fd == -1){
    perror("open destination");
    close(source_fd);
    return -1;
  }

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read;
  int result = 0; // copy_file() 전체 성공/실패 여부 확인
  
  while((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0){
    if(write_all(dest_fd, buffer, bytes_read) == -1){
      perror("write destination");
      result = -1;
      break;
    }
  }

  if(bytes_read == -1){
    perror("read source");
    result = -1;
  }

  if(close(source_fd) == -1){
    perror("close source");
    result = -1;
  }

  if(close(dest_fd) == -1){
    perror("close destination");
    result = -1;
  }

  return result;
}

int main(int argc, char *argv[]){

  if(argc != 3){
    fprintf(stderr, "usage: %s SOURCE DESTINATION\n", argv[0]);
    return EXIT_FAILURE;
  }

  if(copy_file(argv[1], argv[2]) == -1){
    return EXIT_FAILURE;
  }
  
  return 0;
}
