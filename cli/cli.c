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
	int bg = 0;
	//gets(buf);
	//argv[0] = buf;
	while(1)
	{
		gets(buf);
		tp = buf;
		while('\0'!=*tp++)
		{
			if(*tp=='&')
			{
				bg=1;
				break;
			}
		}

		printf("%d\n", bg);

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


		switch(fork())
		{
			case 0:
				execvp(args[0], args);
				printf("something went wrong\n");
			default:
				if(!bg) wait(NULL);
		}
	}
}