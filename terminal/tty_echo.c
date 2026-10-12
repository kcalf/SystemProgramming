// Linux/Unix Terminal ECHO 기능을 켜거나 끄고, 현재 상태를 확인하는 프로그램
// ECHO: 사용자가 키보드로 타이핑한 글자를 터미널 화면에 그대로 보여주는 기능

#include<stdio.h>
#include<string.h>
#include<termios.h>
#include<unistd.h>

int main(int argc, char *argv[]){
  struct termios settings;

  if(argc != 2 || (strcmp(argv[1], "status") != 0 && strcmp(argv[1], "on") != 0 && strcmp(argv[1], "off") != 0)){
    fprintf(stderr, "Usage: %s status|on|off\n", argv[0]);
    return 1;
  }

  if(tcgetattr(STDIN_FILENO, &settings) == -1){ // tcgetattr(STDIN_FILENO, ~): 표준 입력의 상태 정보를 가져옴
    perror("tcgetattr");
    return 1;
  }

  if(strcmp(argv[1], "status") == 0){ // ECHO 플래그가 켜져 있는지 확인
    printf("ECHO: %s\n", settings.c_lflag & ECHO ? "ON" : "OFF");
    return 0;
  }

  if(strcmp(argv[1], "on") == 0){
    settings.c_lflag |= ECHO; // 비트 OR 연산으로 ECHO 플래그를 1로 켬
  }
  else settings.c_lflag &= ~ECHO; // 비트 NOT(~)+AND(&) 연산으로 ECHO 플래그만 0으로 끎

  // tcsetattr: 변경된 settings 구조체의 내용을 시스템에 반영
  // TCSANOW: 지금 즉시 설정을 적용하라
  if(tcsetattr(STDIN_FILENO, TCSANOW, &settings) == -1){ 
    perror("tcsetattr");
    return 1;
  }

  return 0;
}
