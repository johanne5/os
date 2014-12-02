#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1
#define BSIZE 1000

int main(int argc, char **argv)
{
	printf("parent process reporting for duty\n");
	
	char buf[BSIZE];
	
	int fd[2];
	pipe(fd);
	

	int c = fork();

	switch(c)
	{
		case 0: /*child*/
			//close(READ);
			//dup2(fd[WRITE], 1);
			//dup2(fd[WRITE], 2);
			//execlp("bin/cat", "hej", " man", NULL);
			//write(fd[WRITE], "hellooo", BSIZE);
			break;
		default: /*parent*/
			printf("so this happens...\n");
			//read(fd[READ], buf, BSIZE);
			//printf("%s\n", buf);
			close(READ);
			close(WRITE);
			break;
	}

	return 0;
}