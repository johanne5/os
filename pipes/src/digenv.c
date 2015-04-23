#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/*
If this program is called with no arguments, it 
will execute sort on the output from printenv and forward to a pager.
If the PAGER environment variable is set, the program will use that.
Otherwise less. If less fails, the program will use more.
If this program is called with one or more arguments,
they will all be passed to grep which is executed before
sort in the above sequence when program is called with no arguments.
*/

//a struct with slots for 2 file descriptors
typedef struct pipes 
{
	int readEnd;
	int writeEnd;
} pipe_t;

//argc: the number of arguments in the program call
//argv: a string array with parameters
int main(int argc, char **argv)
{
	//commands is a vector containing all the commands to be executed (less and more may not be executed)
	//command is a pointer to traverse commands
	char *const* command, *const commands[] = {"printenv", "grep", "sort", NULL, NULL!=getenv("PAGER") ? getenv("PAGER") : "less", "more"};
	
	//pipes for two iterations
	pipe_t newPipe, oldPipe = (pipe_t) {0,0};

	//child process id
	pid_t cpid;

	//iterates over the commands untill NULL is reached
	for(command = (char **)commands; *command; command++)
	{	
		//if program for this iteration is grep and no arguments have been passed to this program, grep is not executed
		if(1==argc && !strcmp("grep", *command))	
			continue;
		
		//opens a new pipe.
		pipe( (int *)&newPipe);
		//forks the process.
		cpid = fork();
		switch(cpid)	//forking this process
		{
			case -1:
				fprintf(stderr, "Error creating child process\n");
				exit(2);

			//This case is only executed by the child process.
			case 0:		
				
				//redirecting parent's read-end from last iteration to stdin and child process' write-end for this iteration to std out
				do_dup2(oldPipe.readEnd, STDIN_FILENO, "child process dup read end -> stdin");
				do_dup2(newPipe.writeEnd, STDOUT_FILENO, "child process dup write end -> stdout");
				
				//closing childs read and write-end to pipe
				do_close(newPipe.readEnd, "child process new read");
				do_close(newPipe.writeEnd, "child process new write");

				//If the current command is grep, execute with the parameters used when calling this program.
				if(!strcmp("grep", *command))
				{
					argv[0] = "grep";
					do_execvp(argv);
				}
				//Else execute a different command with no parameters.
				else
					do_execvp((char *const[]) {*command, NULL});	
			//This case is only executed by the parent process.	
			default:
				//False the first time since oldPipe's values are zero-initiated.
				if(oldPipe.readEnd)
					do_close(oldPipe.readEnd, "parent process old read");
				//Saving away parent's read-end for this iteration
				oldPipe.readEnd = newPipe.readEnd;
				//Closing parent write-end to pipe
				do_close(newPipe.writeEnd, "parent process new write");
				break;
		}
	}
	
	//wait for all child process.
	do_wait();
	//Redirecting the parent from the last iteration's read-end to stdin.
	do_dup2(newPipe.readEnd, STDIN_FILENO, "only process dup read end -> stdin");
	//Closing the parent from last iteration's read-end.
	do_close(newPipe.readEnd, "last read end");
	
	//Continuing in the list of commands, executing pager specified by environment variable or less
	command++;
	//If this call returns, neither a pager defined by PAGER environment variable nor less exists.
	do_execvp((char *const[]) {*command, NULL});
	//Continuing to more
	command++;
	do_execvp((char *const[]) {*command, NULL});
}

do_close(int fd, const char *clue)
{
	int returnStatus = close(fd);

	if(-1 == returnStatus)
	{
		perror(clue);
		exit(2);
	}
}
do_dup2(int fdOld, int fdNew, const char *clue)
{
	int returnStatus = dup2(fdOld, fdNew);

	if(-1 == returnStatus)
	{
		perror(clue);
		exit(2);
	}
}

do_execvp(char *const argv[], char *const clue)
{
	execvp(argv[0], argv);
	perror(clue);
	exit(2);
}

do_wait()
{
	int status, pid;
	
	while(1)
	{
		pid = wait(&status);
		//If this case happens, there are no more child processes.
		if(-1 == pid)
			break;
		//If this case happens, a child has exited with some exit status.
		else if(WIFEXITED(status))
		{
			//Child process failed.
			if(WEXITSTATUS(status))
			{
				fprintf(stderr, "Child process %d exited normally with status %d.\n", pid, status);
				exit(2);
			}
		}
		//To catch unexpected faults.
		else
		{
			fprintf(stderr, "Some unhandled fault occured when waiting for child process");
			exit(2);
		}
	}
}