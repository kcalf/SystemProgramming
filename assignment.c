// 2025003745 김예원

#define BUFFERSIZE 1024
#define SHOWHOST

#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

static int write_all(int fd, const char *buffer, ssize_t length)
{
	ssize_t written = 0;

	while (written < length)
	{
		ssize_t result = write(fd, buffer + written, (size_t)(length - written));

		if (result == -1)
		{
			return -1;
		}

		written += result;
	}

	return 0;
}

void show_info(struct utmp *ut_buf_p, int dest_fd)
{
	char buffer[BUFFERSIZE];
	time_t sec;
	char *cp;
	int length;

	if (ut_buf_p->ut_type != USER_PROCESS)
		return;

	sec = (time_t)ut_buf_p->ut_tv.tv_sec;
	cp = ctime(&sec);

	if (cp == NULL)
		return;

#ifdef SHOWHOST
	if (ut_buf_p->ut_host[0] != '\0')
	{
		length = snprintf(buffer, sizeof(buffer),
			"%-8.8s %-8.8s %12.12s (%.*s)\n",
			ut_buf_p->ut_user,
			ut_buf_p->ut_line,
			cp + 4,
			(int)sizeof(ut_buf_p->ut_host),
			ut_buf_p->ut_host);
	}
	else
#endif
	{
		length = snprintf(buffer, sizeof(buffer),
			"%-8.8s %-8.8s %12.12s\n",
			ut_buf_p->ut_user,
			ut_buf_p->ut_line,
			cp + 4);
	}

	if (length < 0 || length >= BUFFERSIZE)
		return;

	if (write_all(STDOUT_FILENO, buffer, length) == -1)
	{
		perror("write stdout");
		return;
	}

	if (write_all(dest_fd, buffer, length) == -1)
	{
		perror("write destination");
		return;
	}
}

int main(int argc, char *argv[])
{
	struct utmp current_record;
	int utmpfd;
	int dest_fd;
	size_t rec_len = sizeof(struct utmp);

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <output_filename>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((utmpfd = open("/var/run/utmp", O_RDONLY)) == -1)
	{
		perror(UTMP_FILE);
		exit(-1);
	}

	if ((dest_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
	{
		perror("open destination");
		close(utmpfd);
		return EXIT_FAILURE;
	}

	while (read(utmpfd, &current_record, rec_len) == rec_len)
	{
		show_info(&current_record, dest_fd);
	}

	if (close(utmpfd) == -1)
	{
		perror("close utmp");
		return EXIT_FAILURE;
	}

	if (close(dest_fd) == -1)
	{
		perror("close destination");
		return EXIT_FAILURE;
	}

	return 0;
}
