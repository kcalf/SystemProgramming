// 현재 프로그램이 실행 중인 터미널 창의 크기를 조회하여 출력하는 프로그램

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(void)
{
    struct winsize size;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &size) == -1) {
        perror("ioctl TIOCGWINSZ");
        return 1;
    }
    printf("Rows    : %u\n", (unsigned int)size.ws_row); // ws_row: 터미널의 가로 줄 개수
    printf("Columns : %u\n", (unsigned int)size.ws_col); // ws_col: 한 줄에 들어가는 글자 개수
    printf("Pixels  : %u x %u\n",
           (unsigned int)size.ws_xpixel, (unsigned int)size.ws_ypixel); // ws_xpixel/ws_ypixel: 터미널의 가로/세로 픽셀 크기
    return 0;
}
