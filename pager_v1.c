#define PAGE_SIZE 5
#define BUFFER_SIZE 256

#include<stdio.h>

void display_stream(FILE *source); // 데이터를 읽고 화면에 출력
int read_command(); // 사용자의 Pager 명령 입력 처리

int main(int argc, char *argv[]){
  FILE *source = stdin;

  if(argc > 1){ // 첫 번째 인자 파일만 사용됨 (간략화)
    source = fopen(argv[1], "r");

    if(source == NULL){
      perror("fopen");
      return 1;
    }
  }

  display_stream(source);

  if(source != stdin){ // source가 표준 입력이 아닐 경우에만 fclose()
    fclose(source);
  }

  return 0;
}

void display_stream(FILE *source){
  char line[BUFFER_SIZE];
  int line_count = 0; // line_count: 현재 몇 줄 출력했는지 확인
  int advance; // advance: 사용자가 몇 줄을 더 보고 싶어 하는지 저장 

  while(fgets(line, sizeof(line), source) != NULL){ // source에서 한 문자열씩 읽음
    if(line_count == PAGE_SIZE){ // 현재까지 PAGE_SIZE만큼 출력했는지 확인
      advance = read_command(); 

      if(advance == 0){
        break;
      }

      line_count -= advance;
    }

    fputs(line, stdout);
    line_count++;
  }
}

int read_command(){ 
  int cmd;

  printf("\033[7m more? \033[0m");
  fflush(stdout);

  while((cmd = getchar()) != EOF){
    if(cmd == 'q') return 0;
    if(cmd == ' ') return PAGE_SIZE;
    if(cmd == '\n') return 1;
  }
}
