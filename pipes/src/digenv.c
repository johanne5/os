#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define READ fd[0]
#define WRITE fd[1]
#define BSIZE 100000

int main(int argc, char **argv)
{
	
	char buf[BSIZE];
	
	int fd[2];
	pipe(fd);
	
	for(int i=0; i<2; i++)
	{
		switch(fork())
		{
		case 0: /*child*/
			switch(i)
			{
				case 0:
					close(READ);
					dup2(WRITE, 1);
					//execl("/usr/bin/printenv", "printenv", NULL);
					execl("/usr/bin/printenv", "printenv", NULL);
				case 1:
					close(READ);
					dup2(WRITE, 1);
					execl("/bin/sort", "sort", "c\nb\na\n", NULL);
			}
			
		default: /*parent*/

			read(READ, buf, BSIZE);
			//write(WRITE,buf, BSIZE);
			if(1==i)
			{
			printf("%s\n", buf);
			}
			break;
		}
	}
	
	return 0;
}