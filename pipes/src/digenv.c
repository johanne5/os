#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define READ fd[0]
#define WRITE fd[1]
#define BSIZE 100000

int main(int argc, char **argv)
{

	const char *printenv[] = {"printenv", NULL};
	const char *sort[] = {"sort", NULL};
	const char *echo[] = {"echo", "c\nb\na\n", NULL};
	const char **cmd[] = {echo, sort, NULL};
	
	char buf[BSIZE];
	FILE *str = tmpfile();
	// FILE *str = fopen("tmp", "a");
	
	int fd[2], fd_in = 0;

	

	int i = 0;
	while(*cmd[i] != NULL) {
		i++;
		pipe(fd);
		switch(fork())
		{
		case 0: /*child*/
			// switch(i)
			// {
			// 	// case 0:
			// 	// // close(READ);
			// 	// dup2(WRITE, 1);
			// 	// execvp((*cmd)[0], *cmd);
			// 	// 	//execl("/usr/bin/printenv", "printenv", NULL);
			// 	// // execl("/usr/bin/printenv", "printenv", NULL);
			// 	// //execl("/bin/echo", "echo", "c\nb\na\n", NULL);
			// 	// case 1:
			// 	// dup2(WRITE, 1);
			// 	// dup2(READ, 0);
			// 	// execl("/usr/bin/sort", "sort", NULL);
			// 	// // execl("/bin/echo", "echo", "hej hej hej hej hej hej", NULL);
			// }
			dup2(fd_in, 0);
			dup2(WRITE, 1);
			break;
			//execvp((*cmd)[i], *cmd);


		default: /*parent*/

			//memset(buf, 0, BSIZE);
			//read(READ, buf, BSIZE);
			// fputs(buf, str);
			printf("hi\n");
			wait(NULL);
			fd_in = WRITE;
		}

	}
	read(READ, buf, BSIZE);
	printf("%s\n", buf);
	
	
	return 0;
}