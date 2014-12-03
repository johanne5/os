#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ fd[0]
#define WRITE fd[1]
#define BSIZE 100000

int main(int argc, char **argv)
{
	
	char grepargs[1000];

	int i=0;
	argc--;
	while(0!=argc--){
		
		while('\0'!=argv[1][0])
		{
			grepargs[i++] = argv[1][0];
			argv[1]++;
		}
		if(0!=argc)
		{
			grepargs[i++] = '\\';
			grepargs[i++] = '|';
		}
		
		argv++;
	}

	const char *printenv[] = {"printenv", NULL};
	const char *sort[] = {"sort", NULL};
	const char *grep[] = {"grep", grepargs, NULL};
	const char *less[] = {"less", "-", NULL};
	const char *echo[] = {"echo", "hej\napa\n", NULL};

	const char **commands[] = {printenv, grep, sort, NULL};
	const char ***cmd = commands;
	char buf[BSIZE];
	
	int fd[2], readbackup;
	

	while(NULL != *cmd)
	{
		pipe(fd);
		switch(fork())
		{
			case 0:
				
				dup2(readbackup, 0);
				dup2(WRITE, 1);
				close(WRITE);
				close(READ);
				execvp(cmd[0][0],(char *const*) cmd[0]);
			default:
				wait(NULL);
				readbackup = READ;
				close(WRITE);
				cmd = cmd + 1;			
		}

	}
	close(WRITE);
	dup2(READ, 0);
	execvp(less[0], (char *const*) less);
}