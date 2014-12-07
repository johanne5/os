#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
							//macros for readability
#define READ fd[0]			//macro for file descriptor's read end
#define WRITE fd[1]			//macro for file descriptor's write end

/*
If this program is called with no arguments, it will execute sort on the output from printenv and forward to a pager.
If the PAGER environment variable is set, the program will use that. Otherwise less. If less fails, the program will use more.
If this program is called with one or more arguments, they will all be passed to grep which is executed before sort in the above sequence when program is called with no arguments.
*/

//argc: the number of arguments in the program call
//argv: a string array with parameters
int main(int argc, char **argv)
{
	argv[0] = (char *) "grep";					//changing the first argument which is something like ./digenv to grep in preparation for call to grep
	const char **grep = (const char**) argv;	//string array for call to grep

	const char *pager[] = {NULL!=getenv("PAGER") ? getenv("PAGER") : "less", NULL};	//string array for call to the pager (user defined or less)	

	const char *printenv[] = {"printenv", NULL}; //string array for call to printenv
	const char *sort[] = {"sort", NULL};	//string array for call to sort

	const char **commands[] = {printenv, grep, sort, NULL};	//list of programs to run before print-out to pager
	const char ***cmd = commands;	//pointer used to traverse the list of programs

	int fd[2], readbackup;	//two integers for the filedescriptors read and write ends respectively, one to pass read end between pipes
	
	/* Loop to iterate over the programs to call. The list is null-terminated */
	while(NULL != cmd[0])
	{
		if(1==argc && !strcmp("grep", cmd[0][0]))	//if program for this iteration is grep and no arguments have been passed to this program, grep is not run
		//if(1==argc && "grep" == cmd[0][0])
		{
			cmd++;
			continue;
		}

		pipe(fd);	//creating new pipe with our file descriptors
		switch(fork())	//forking this process
		{
			case 0:		//only the child process runs this case
				dup2(readbackup, 0);	//copying our saved file descriptor's read end to standard input
				dup2(WRITE, 1);	//copying our file descriptor's write end to standard output
				close(WRITE);	//closing our channel to the file descriptor's write end
				close(READ);	//closing our channel to the file descriptor's read end
				execvp(cmd[0][0],(char *const*) cmd[0]); //executing a command from the command's list
			default:	//only the parent process runs this case
				wait(NULL);	//waiting for the child process to terminate
				readbackup = READ; //backing up the read end of our file descriptor
				close(WRITE); //closing our channel to the file descriptor's write end
				cmd++; //incrementing the pointer to the command's list
		}
	}
	dup2(readbackup, 0);	//copying our saved file descriptor's read end to standard input
	close(READ);	//closing our channel to the file descriptor's read end
	execvp(pager[0], (char *const*) pager);	//executing call to the pager
	pager[0]="more"; //if user defined pager was not defined or less did not work, change pager to more
	execvp(pager[0], (char *const*) pager); //retry executing call to the pager
}