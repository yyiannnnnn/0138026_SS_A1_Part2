#include "header.h"

/* discards the rest of the current input line */
void clearing_buffer_STDIN(void)
{
	int c;

	while ((c = getchar()) != '\n' && c != EOF)
		;
}

/*
Prints the prompt and reads one line (newline stripped).
Over-long input is truncated and the leftover is discarded.
Exits cleanly if stdin is closed.
*/
void read_line(const char *prompt, char *buf, size_t size)
{
	printf("%s", prompt);
	fflush(stdout);
	if (fgets(buf, size, stdin) == NULL)
	{
		printf("\nInput closed. Exiting.\n");
		exit(1);
	}
	if (strchr(buf, '\n') == NULL)
		clearing_buffer_STDIN();
	buf[strcspn(buf, "\r\n")] = '\0';
}

/* same as read_line but terminal echo is turned off */
void read_password(const char *prompt, char *buf, size_t size)
{
	struct termios old, noecho;
	int tty = isatty(STDIN_FILENO);

	if (tty)
	{
		tcgetattr(STDIN_FILENO, &old);
		noecho = old;
		noecho.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &noecho);
	}
	read_line(prompt, buf, size);
	if (tty)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &old);
		printf("\n");
	}
}
