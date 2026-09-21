// 2025003745 김예원

#define SHOWHOST
#define OUTPUT_BUFSIZE 1024

#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

static int write_all(int fd, const char *buffer, size_t length)
{
    size_t written = 0;

    while (written < length)
    {
        ssize_t result =
            write(fd, buffer + written, length - written);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        written += (size_t)result;
    }

    return 0;
}

static int show_info(const struct utmp *ut_buf_p, int dest_fd)
{
    char time_buf[32];
    char line_buf[OUTPUT_BUFSIZE];

    struct tm tm_buf;
    time_t sec;

    int len;

    if (ut_buf_p->ut_type != USER_PROCESS)
        return 0;

    sec = (time_t)ut_buf_p->ut_tv.tv_sec;

    if (localtime_r(&sec, &tm_buf) == NULL)
    {
        strcpy(time_buf, "??? ?? ??:??");
    }
    else
    {
        strftime(
            time_buf,
            sizeof(time_buf),
            "%b %e %H:%M",
            &tm_buf
        );
    }


#ifdef SHOWHOST
    if (ut_buf_p->ut_host[0] != '\0')
    {
        len = snprintf(
            line_buf,
            sizeof(line_buf),

            "%-8.8s %-8.8s %s (%.*s)\n",

            ut_buf_p->ut_user,
            ut_buf_p->ut_line,
            time_buf,

            (int)sizeof(ut_buf_p->ut_host),
            ut_buf_p->ut_host
        );
    }
    else

#endif
    {
        len = snprintf(
            line_buf,
            sizeof(line_buf),

            "%-8.8s %-8.8s %s\n",

            ut_buf_p->ut_user,
            ut_buf_p->ut_line,
            time_buf
        );
    }
	
    if (len < 0 || (size_t)len >= sizeof(line_buf))
    {
        fprintf(stderr, "output line is too long\n");
        return -1;
    }

    if (write_all(
            STDOUT_FILENO,
            line_buf,
            (size_t)len) == -1)
    {
        perror("write stdout");
        return -1;
    }

    if (write_all(
            dest_fd,
            line_buf,
            (size_t)len) == -1)
    {
        perror("write output file");
        return -1;
    }

    return 0;
}


int main(int argc, char *argv[])
{
    struct utmp current_record;

    int utmpfd;
    int dest_fd;

    ssize_t nread;
    size_t rec_len = sizeof(struct utmp);

    int status = EXIT_SUCCESS;

    if (argc != 2)
    {
        fprintf(
            stderr,
            "Usage: %s <output_filename>\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    utmpfd = open(UTMP_FILE, O_RDONLY);

    if (utmpfd == -1)
    {
        perror(UTMP_FILE);
        return EXIT_FAILURE;
    }

    dest_fd = open(
        argv[1],
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (dest_fd == -1)
    {
        perror("open output file");

        close(utmpfd);

        return EXIT_FAILURE;
    }

    while ((nread =
                read(
                    utmpfd,
                    &current_record,
                    rec_len
                ))
           == (ssize_t)rec_len)
    {
        if (show_info(
                &current_record,
                dest_fd) == -1)
        {
            status = EXIT_FAILURE;
            break;
        }
    }

    if (nread == -1)
    {
        perror("read utmp");
        status = EXIT_FAILURE;
    }
    else if (
        nread != 0 &&
        nread != (ssize_t)rec_len)
    {
        fprintf(
            stderr,
            "incomplete utmp record\n"
        );

        status = EXIT_FAILURE;
    }

    if (close(utmpfd) == -1)
    {
        perror("close utmp");
        status = EXIT_FAILURE;
    }

    if (close(dest_fd) == -1)
    {
        perror("close output file");
        status = EXIT_FAILURE;
    }


    return status;
}
