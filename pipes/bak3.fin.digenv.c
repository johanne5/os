#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //fork, read
#include <sys/wait.h> //wait, duh

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]){
	fprintf(stderr, "argv[0]: %s\n", argv[0]);
	fprintf(stderr, "argv[1]: %s\n", argv[1]);
	fprintf(stderr, "argc: %d\n\n", argc);

	char* cmds[4] = {"printenv", "grep", "sort", "less"};
	
	int tmp = -1, filedesc[2];
	pipe(filedesc);
	
	int i;
	for(i = 0; i < 3; i++){

		if(argc == 1) { /* no arguments, no grep */
			if(i == 1) {
				continue;
			}
		}

		//printenv
		int pid = fork();
		switch(pid)
		{
			case 0: /* child */
			{
				fprintf(stderr, "Child process\n");
				fprintf(stderr, "child tmp is %d\n", tmp);

				close(filedesc[READ]);

				if(i > 0){
					int ret1 = dup2(tmp, fileno(stdin)); /* not checked */
					fprintf(stderr, "ret1: %d\n", ret1);
					close(tmp);
				}

				int ret2 = dup2(filedesc[WRITE], fileno(stdout)); /* not checked */
				//int ret2 = dup2(fileno(stdout), filedesc[WRITE]); /* not checked */
				fprintf(stderr, "ret2: %d\n", ret2);
				
				fprintf(stderr, "stdout: %d\nfiledesc[WRITE]: %d\n", fileno(stdout), filedesc[WRITE]);

				close(filedesc[WRITE]);

				if(i != 1){
					if(i == 10){
						fprintf(stderr, "%s %s %s %s\n", cmds[i], cmds[i], "hello", "world");
						execlp("echo", "echo", "hello", "world", (char*) 0);
					}

					fprintf(stderr, "%s %s\n", cmds[i], cmds[i]);
					execlp(cmds[i], cmds[i], (char*) 0);
				} else {
					argv[0] = "grep";

					fprintf(stderr, "%s ", cmds[i]); 
					int t = 0;
					while(t < argc){
						fprintf(stderr, "%s ", argv[t++]); 
					}
					fprintf(stderr, "\n");

					execvp(cmds[i], argv);
				}
			}
			case -1:
			{
				fprintf(stderr, "Error in fork()\n");
				return 1;
			}
			default:
			{
				fprintf(stderr, "Parent: Pid was %d\n", pid);

				close(filedesc[WRITE]);
				if(i > 0){
					close(tmp);
				}

				int status;
				int pid2 = wait(&status);
				if(pid == pid2){
					fprintf(stderr, "Child %d returned with status %d\n",pid2,status);
					
					tmp = filedesc[READ];
					fprintf(stderr, "parent tmp is %d\n", tmp);

					pipe(filedesc);
				}
			}
		}
	}
	close(filedesc[0]);
	close(filedesc[1]);
	int ret1 = dup2(tmp, fileno(stdin)); /* not checked */
	fprintf(stderr, "ret1: %d\n", ret1);
	close(tmp);

	fprintf(stderr, "%s %s\n", cmds[3], cmds[3]);
	execlp(cmds[3], cmds[3], (char*) 0);
	return 0;
}

/*
//to print 128 chars from filedesc 'tmp'
char buf[128];
int i = 0;
while(i < 128){
	buf[i++] = 0;
}
int ret2 = read(tmp, buf, 128);
fprintf(stderr, "tmp: %s\n", buf);
*/
