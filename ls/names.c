#include <stdio.h> // 표준 입출력 함수 (fprintf, puts, perror 등)를 위한 헤더
#include <dirent.h> // 디렉터리 탐색 관련 함수 및 구조체 (DIR, dirent, opendir 등)를 위한 헤더
#include <errno.h> // 오류 처리 변수 (errno)를 사용하기 위한 헤더

int main(int argc, char *argv[])
{
    const char *directory;
    DIR *stream; // 디렉터리 스트림을 가리킬 포인터
    struct dirent *entry; // 디렉터리 내의 각 항목(파일/폴더) 정보를 담을 구조체 포인터
    int status = 0; // 프로그램 최종 종료 상태를 저장할 변수 (0은 성공)

    if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        return 1;
    }

    directory = argc == 2 ? argv[1] : ".";
    stream = opendir(directory);
    if (stream == NULL)
    {
        perror(directory); // 디렉터리를 여는 데 실패하면(권한 없음, 존재하지 않음 등) 에러 메시지 출력
        return 1;
    }

    for (;;)
    {
        errno = 0;
        entry = readdir(stream);
        if (entry == NULL) // 더 이상 읽을 파일이 없거나 오류가 발생한 경우 NULL 반환
        {
            if (errno != 0)
            {
                perror(directory);
                status = 1;
            }
            break;
        }
        puts(entry->d_name); // 읽어온 항목의 이름(파일명 또는 폴더명)을 콘솔에 출력
    }

    if (closedir(stream) == -1)
    {
        perror(directory);
        status = 1;
    }

    return status;
}
