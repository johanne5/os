/*Programmet exekverar kommandon som skrivs i prompten.
Programmets working directory ändras med kommandot:
	cd [dir]
Om dir utelämnas ändras processens working directory
till användarens home folder.

Programmet avslutas med kommandot exit.*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>


/*Om programmet kompileras med konstanten SIGNALDETECTION
definierad används signaler för att avsluta zombies.
Annars körs waitpid mellan varje kommando som städar upp
zombies.*/
#define BUFSIZE 70
#ifndef SIGNALDETECTION
	#define SIGNALDETECTION 0
#else
	#if(1!=SIGNALDETECTION)
		#define SIGNALDETECTION 0
	#endif
#endif

void zombieslayer(void);
void INThandler(int);
void zombiehandler(int);
void CHLDhandlerreg(void);
void fgwait(pid_t pid);
int sigrelse(int sig);
int sighold(int sig);


int main(int argc, char *argv[])
{
	char buf[BUFSIZE]; //buffer for reading command
	char *args[5]; //string array for parsed command
	char ** sp; //temp pointer to traverse args
	char * tp; //tmp pointer to traverse buf
	int bg; //boolean, 0 if command is to be exec foreground


	signal(SIGINT, INThandler); //registering handler for SIGINT
	if(SIGNALDETECTION) CHLDhandlerreg(); //selektiv registrering av SIGCHLD handler

	/*Roughly prompts user for command, forks the process and executes
	command either in foreground or background.*/
	while(1)
	{
		if(!SIGNALDETECTION)
			zombieslayer(); //if zombies are not signalhandled, clean zombies
		if(!fgets(buf, BUFSIZE, stdin)) //prompt user for command
		{
			printf("failed reading command");
			exit(1);
		}

		tp = buf;
		while('\n' != *tp)
			tp++; //finds the new line. Commands are executed when user hits enter.
		*tp = '\0'; //removes the new line by ending string on that position

		if(('\0'==*buf || ' '==*buf) ) //checking for illegal commands i.e space and cr
		{
			printf("Ouch! That hurts!\n");
			continue;
		}

		for(tp = buf, bg = 0; '\0' != *tp && '&' != *tp; tp++); //searches command for '&' and sets the variable bg accordingly
		if(*tp=='&')
		{
			*tp=' ';
			bg=1;
		}	

		sp=args;
		*sp = strtok(buf, " "); //setting buf to split on space
		sp++;

		/*parsing the command and initialising the string array args*/
		do
		{
			*sp = strtok(NULL, " "); //assigning next substring to args
			sp++;
		} while (sp[-1]);


		if(!strcmp("exit", args[0])) exit(0); //built in command
		if(!strcmp("cd", args[0])) //built in command
		{
			if(!args[1]) //if no param. to cd, set working dir to HOME
				if(-1==chdir(getenv("HOME")))
				{
					printf("failed to change working directory\n");
					exit(1);
				}
			else
				if(-1==chdir(args[1]))
				{
					printf("failed to change working directory\n");
					exit(1);
				}
			continue;
		}

		pid_t pid;
		/* forking the proccess, letting child process execute command */
		switch(pid = fork())
		{
			case 0:
				execvp(args[0], args);
				printf("something went wrong with child process\n");
				exit(1);
			case -1:
				printf("child is DOA!!");
				exit(1);

			default:
				printf("Created %s process %d\n", bg==0 ? "foreground" : "background", pid);
				if(!bg) fgwait(pid); //if foregroundprocess, wait
				break;
		}	
	}
	return 0;
	
}

/*slays zombies that have terminated*/
void zombieslayer(void)
{
	pid_t zpid; //child process ID
	int status;

	/*While there are more zombies, keep slaying them*/
	while( (zpid = waitpid(-1, &status, WNOHANG)) > 0 )
	{
		if(status)
		{
			printf("%d exited with status %d\n", zpid, status);
		}
		else
		{
			printf("%d terminated normally\n", zpid);
		}
	}
}

/*Ignoring SIGINT*/
void INThandler(int sig)
{
	printf("Ignored SIGINT!!\n" );

}

/*same as zombieslayer, but as signal handler for SIGCHLD*/
void zombiehandler(int sig)
{
	pid_t zpid;
	int status;
	while( (zpid = waitpid(-1, &status, WNOHANG)) > 0 )
	{
		if(status)
		{
			printf("%d exited with status %d [signal] \n", zpid, status);
		}
		else
		{
			printf("%d terminated normally [signal]\n", zpid);
		}
	}
}

/*SIGCHLD handler registerer*/
void CHLDhandlerreg(void)
{
	struct sigaction sa; //struct for use with sigaction
	sa.sa_handler = &zombiehandler; //setting handler to our custom handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

	if(sigaction(SIGCHLD, &sa, 0) == -1) //registering handler
	{
		printf("Error sigactionthingy\n");
		exit(1);
	}
}

/*waiting for foreground process to terminate*/
void fgwait(pid_t pid)
{
	sighold(SIGCHLD); //holds SIGCHLD signals
	sighold(SIGINT); //holds SIGINT signals

	int status = 0;

	struct timeval tv; //struct for use with gettimeofday
	time_t t; //the time variable

	if(gettimeofday(&tv, NULL))
	{
		printf("gettimeofday went wrong!!");
		exit(1);
	}
	t=tv.tv_usec; //set time to the value in tv

	if(-1==waitpid(pid, &status,0)) //wait for the child process to terminate
	{
		printf("Something went wrong when waiting for fg process\n");
	}

	if(gettimeofday(&tv, NULL)) //update the time in tv
	{
		printf("gettimeofday went wrong!!");
		exit(1);
	}

	//printout normal termination or status
	if(status)
	{
		printf("%d exited with status %d", pid, status);
	}
	else
	{
		printf("%d terminated normally", pid);
	}

	printf(" after %ld usec\n", (tv.tv_usec-t)); //printing time difference
	sigrelse(SIGINT); //reenabling the signals
	sigrelse(SIGCHLD);
}
