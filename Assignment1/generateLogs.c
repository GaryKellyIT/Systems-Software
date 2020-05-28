#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


void exec1();
void exec2();


int pipefd[2];
pid_t pid;

int generateLogs()
{
	
	//system("/sbin/ausearch -f /var/www/html/Intranet | /sbin/aureport -f -i > /home/gary/Documents/auditlogs.txt")
	//create pipe
	if (pipe(pipefd) == -1 /*|| pipe(pipefd2) == -1*/){
		perror("Error init pipe generateLogs.c");
		exit(1);
	}

	//create child process
	if((pid = fork()) ==-1){
		perror("Error init fork generateLogs.c");
		exit(1);
	}
	else if(pid==0)
	{
		exec1();	
	}
	
		
	if((pid = fork()) ==-1){
		perror("Error init fork generateLogs.c");
		exit(1);
	}
	else if(pid==0)
	{
		exec2();	
	}
	
	close(pipefd[0]);
	close(pipefd[1]);
	
}

void exec1() {
	dup2(pipefd[1],1);

	close(pipefd[0]);
	close(pipefd[1]);

	execlp("/sbin/ausearch","/sbin/ausearch","-f","/var/www/html/Intranet",NULL);
	//exec didn't work,exit
	perror("Error with /sbin/ausearch");
	exit(1);
}

void exec2() {
	dup2(pipefd[0],0);

	int fd = open("auditlogs.txt", O_WRONLY|O_CREAT, 0666);
	dup2(fd,1);
	close(fd);

	close(pipefd[0]);
	close(pipefd[1]);

	execlp("/sbin/aureport","/sbin/aureport","-f","-i",(char*)0);
	//exec didn't work,exit
	perror("Error with /sbin/aureport");
	exit(1);
}

