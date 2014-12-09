#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFSIZE 70



int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	char *args[5];
	char ** sp, * tp;
	int bg;
	//gets(buf);
	//argv[0] = buf;
	while(1)
	{
		gets(buf);
		tp = buf;
		bg=0;
		while('\0'!=*tp++)
		{
			if(*tp=='&')
			{
				*tp=' ';
				bg=1;
				break;
			}
		}

		sp=args;
		sp[0] = strtok(buf, " ");
		sp++;

		do
		{
			sp[0] = strtok(NULL, " ");
			sp++;
		} while (NULL!=sp[-1]);

		if(!strcmp("exit", args[0])) exit(0);
		if(!strcmp("cd", args[0]))
		{
			if(NULL==args[1]) chdir(getenv("HOME"));
			else chdir(args[1]);
			continue;
		}

		pid_t pid;
		int status;
		switch(pid = fork())
		{
			case 0:
				execvp(args[0], args);
				printf("something went wrong\n");
				exit(1);
			default:
			
				//wait(NULL);
				
				status=0;
				if(!bg)
				{
					printf("%d\n", pid);
					printf("%d\n", waitpid(pid, &status, WEXITSTATUS) );
					
				}
		}
	}
}