// 터미널의 설정 상태(보드레이트, 특수 제어 문자, 각종 제어 플래그)를 조회하여 화면에 보기 좋게 출력하는 프로그램

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct flag_entry {
    tcflag_t mask;
    const char *name;
};

static const struct flag_entry input_flags[] = {
    { ICRNL, "ICRNL" }, // 엔터 키 입력 시 CR(Carriage Return)을 NL(New Line)로 변환할지 여부
    { IXON,  "IXON" } //  Ctrl+s/Ctrl+o를 통한 소프트웨어 흐름 제어 활성화 여부
};

static const struct flag_entry local_flags[] = {
    { ISIG,   "ISIG" }, // Ctrl+C, ctrl+Z 같은 인터럽트 신호 키 작동 여부
    { ICANON, "ICANON" }, // 정규 모드(엔트를 누를 때까지 버퍼링하고 한 줄씩 입력받음) 활성화 여부 
    { ECHO,   "ECHO" }, // 사용자가 키보드로 타이핑한 글자를 화면에 그대로 보여줄지 여부
    { ECHOE,  "ECHOE" } // 백스페이스를 눌렀을 때 화면의 글자를 지울지 여부
};

static void print_speed(const struct termios *settings) // 터미널 통신 속도 출력
{
    speed_t speed = cfgetospeed(settings); // 터미널의 출력 속도(Baud Rate) 코드를 가져옴
    unsigned int baud;

    puts("[Terminal Speed]");
    switch (speed) { // 가져온 코드가 미리 정의된 매크로 중 어디에 해당하는지 비교
    case B300:   baud = 300;   break;
    case B600:   baud = 600;   break;
    case B1200:  baud = 1200;  break;
    case B2400:  baud = 2400;  break;
    case B9600:  baud = 9600;  break;
    case B38400: baud = 38400; break;
    default:
        printf("Output Baud Rate : Unmapped (code %lu)\n", 
               (unsigned long)speed); 
        return;
    }
    printf("Output Baud Rate : %u\n", baud); // 사람이 읽기 쉬운 값으로 변환하여 출력
}

static void print_control_chars(const struct termios *settings) // 특수 제어 문자 출력
{
    puts("\n[Control Characters]");
    printf("ERASE : ASCII %u\n", (unsigned int)settings->c_cc[VERASE]);
    printf("KILL  : ASCII %u\n", (unsigned int)settings->c_cc[VKILL]);
    // 특수 키 설정의 ASCII 코드 값 출력
}

static void print_flag_group(const char *title, tcflag_t flags,
                             const struct flag_entry entries[],
                             size_t count) // flag ON/OFF 판별 및 출력
{
    printf("\n[%s]\n", title);
    for (size_t i = 0; i < count; ++i)
        printf("%-6s : %s\n", entries[i].name,
               flags & entries[i].mask ? "ON" : "OFF");
}

int main(void)
{
    struct termios settings;

    if (tcgetattr(STDIN_FILENO, &settings) == -1) { // 현재 터미널 설정을 settings 구조체로 받음
        perror("tcgetattr");
        return 1;
    }
    // 각 함수를 호출하여 터미널 상태를 화면에 출력
    print_speed(&settings);
    print_control_chars(&settings);

    // 배열 크기를 동적으로 계산(sizeof)하여 함수에 전달
    print_flag_group("Input Flags", settings.c_iflag, input_flags,
                     sizeof input_flags / sizeof input_flags[0]);
    print_flag_group("Local Flags", settings.c_lflag, local_flags,
                     sizeof local_flags / sizeof local_flags[0]);
    return 0;
}
