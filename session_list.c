/* 
Linux가 관리하는 현재 로그인 세션 정보 파일(/var/run/utmp)을 직접 읽어서
현재 로그인한 사용자들의 이름·터미널·시간·접속 Host를 출력하는 작은 who 명령어 구현

[전체 흐름 요약]
로그인 정보 파일을 연다
→ 사용자 기록 하나를 읽는다
→ 실제 로그인 사용자라면 출력한다
→ 다음 기록을 읽는다
→ 파일 끝까지 반복한다
→ 파일을 닫는다

출력 예시: chick   pts/0   2026-09-21 13:25  (192.168.0.10)
*/

#include<stdio.h> // 화면 출력 및 Error 출력: printf(), fprintf(), perror(), putchar()
#include<stdlib.h> // 프로그램 종료 상태: EXIT_FAILURE
#include<fcntl.h> // 파일 열기 : open(), O_RDONLY
#include<unistd.h> // Unix System Call: read(), close(), ssize_t
#include <utmp.h> // 로그인 세션 구조: struct utmp, USER_PROCESS, UTMP_FILE
#include<time.h> // 시간 처리: time_t, struct tm, localtime(), strftime()

// static: 파일 내부에서만 사용할 함수임을 명시
static void print_session(const struct utmp *entry){ // entry: struct utmp 구조체의 주소를 저장하는 Pointer. const: entry가 가리키는 구조체의 내용을 수정X
  if(entry->ut_type != USER_PROCESS){ // entry가 가리키는 struct utmp 안의 ut_type
    return ;
  }

  printf("%-*.*s  ", UT_NAMESIZE, UT_NAMESIZE, entry->ut_user); // 사용자 이름 출력
  printf("%-*.*s  ", UT_LINESIZE, UT_LINESIZE, entry->ut_line); // Terminal 출력 (ex. pts/0, tty2)
  // %-*.*: -: 왼쪽 정렬, 첫 번째 *: 출력 폭, 두 번째 *: 문자열에서 최대 몇 글자까지 읽을지
  // 만약 UT_NAMESIZE = 32, 최소 출력 폭 = 32, 최대 출력 문자 수 = 32, 왼쪽 정렬
  // struct_utmp의 문자 배열은 일반적인 C 문자열처럼 항상 '\0'으로 끝난다고 보장 못 함
  // %s를 쓰면 배열 범위를 넘어 읽을 가능성 존재

  time_t login_time = (time_t) entry->ut_tv.tv_sec;
  // ut_tv: 로그인 시간이 기록된 필드
  // ut_tv.tv_sec: 로그인 시간이 Unix Timestamp 기준으로 몇 초인지
  // time_t 타입캐스팅으로 변환하여 저장
  struct tm *tm_info = localtime(&login_time);
  // time_t는 사람이 읽기 어려우므로 localtime()으로 연도, 월, 일, 시, 분, 초로 나눔
  // 이 값들을 담는 구조체가 struct tm
  // Unix Timestamp → time_t → localtime() struct tm

  if(tm_info!=NULL){ // localtime()은 실패할 경우 NULL을 반환하므로 확인 필요
    char time_buf[32];

    if(strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", tm_info) > 0){
      // strftime(): 시간을 원하는 문자열 형식으로 만들어주는 함수. ex. 2026-09-21 13:25
      // 기록한 문자 수를 반환함. 실패하거나 공간이 충분하지 않을 경우 0
      printf("%s", time_buf);
    }
  }

  if(entry->ut_host[0] != '\0'){
    // Local Terminal이면 Host가 비어 있을 수도 있음 ('\0')
    // Host가 있을 때만 출력
    printf("  (%.*s)", UT_HOSTSIZE, entry->ut_host);
  }

  putchar('\n'); // 줄바꿈
}

int main(){

  struct utmp entry; // 현재 읽고 있는 Record 한 개 저장
  ssize_t bytes_read; // read()가 실제로 몇 Byte 읽었는지 저장. read() 반환형: ssize_t
  int fd = open(UTMP_FILE, O_RDONLY); // UTMP_FILE 파일을 읽기 전용(O_RDONLY)으로 열기. UTMP_FILE: utmp 파일 경로
  // fd: File Descriptor. open() 성공시 정수 반환
  
  if(fd == -1){ // open() 실패시 -1 반환
    perror("open utmp"); // System Call이 실패한 원인을 사람이 읽을 수 있는 문장으로 출력
    return EXIT_FAILURE;
  }

  while((bytes_read = read(fd, &entry, sizeof(entry))) > 0){
    // fd가 가리키는 utmp 파일에서 struct utmp 하나의 크기만큼 읽어서 entry에 저장함.
    
    if(bytes_read != (ssize_t)sizeof(entry)){ // bytes_read는 ssize_t 타입이므로 타입캐스팅
      fprintf(stderr, "incomplete utmp record\n");
      close(fd);
      return EXIT_FAILURE;
    }

    print_session(&entry);
  }

  if(bytes_read == -1){
    perror("read utmp");
    close(fd); // 파일 사용이 끝났으면 열린 File Desciptor를 닫아야 함
    return EXIT_FAILURE;
  }

  if(close(fd)== -1){
    perror("close utmp");
    return EXIT_FAILURE;
  }
    
  return 0;
}
