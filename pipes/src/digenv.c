#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define read 0
#define write 1

int main(int argc, char **k)
{
	int fd[2];
	pipe(fd);

	pid_t pid, c;

	c = fork();

	pid = getpid();

}