#include <stdio.h>
#include <ctype.h>

int main() {
    int input;
    size_t index = 0;

    puts("Input: type ASCII text, then press Enter.");
    while ((input = getchar()) != EOF) {
        if (input == '\n')
            printf("[%zu] NEWLINE ASCII %d\n", index, input);
        else if (isprint((unsigned char)input))
            printf("[%zu] '%c'     ASCII %d\n", index, input, input);
        else
            printf("[%zu] BYTE    ASCII %d\n", index, input);
        ++index;
    }
    if (ferror(stdin)) {
        perror("getchar");
        return 1;
    }
    return 0;
}
