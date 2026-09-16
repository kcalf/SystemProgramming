#define PAGE_SIZE 5
#define BUFFER_SIZE 256

#include<stdio.h>

void display_stream(FILE *source);
int read_command(FILE *command_stream);

int main(int argc, char *argv[])
{
    FILE *source = stdin;

    if (argc > 1) {
        source = fopen(argv[1], "r");

        if (source == NULL)
            return 1;
    }

    display_stream(source);

    if (source != stdin)
        fclose(source);

    return 0;
}

void display_stream(FILE *source)
{
    char line[BUFFER_SIZE];
    int line_count = 0;
    int advance;

    FILE *tty = fopen("/dev/tty", "r");

    if (tty == NULL) {
        fprintf(stderr, "cannot open /dev/tty\n");
        return;
    }

    while (fgets(line, sizeof(line), source) != NULL) {

        if (line_count == PAGE_SIZE) {
            advance = read_command(tty);

            if (advance == 0)
                break;

            line_count -= advance;
        }

        fputs(line, stdout);
        line_count++;
    }

    fclose(tty);
}

int read_command(FILE *command_stream)
{
    int cmd;

    printf("\033[7m more? \033[0m");
    fflush(stdout);

    while ((cmd = getc(command_stream)) != EOF) {
        if (cmd == 'q') return 0;
        if (cmd == ' ') return PAGE_SIZE;
        if (cmd == '\n') return 1;
    }

    return 0;
}
