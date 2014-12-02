#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
int main(int argc, char c)
{
	int *r, *w;
	int *fd[2] = {r, w};
	pipe(fd);

	pid_t pid, c;

	c = fork();

	pid = getpid();
}