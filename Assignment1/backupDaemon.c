#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#define MAX_BUF 1024

int main()
{
	char folder[100] = "/home/gary/Documents/TestFolder";
	int fd;
	char * fifoFile = "/tmp/fifoFile";
	char buf[MAX_BUF];


	
	
	time_t now;
	struct tm backupFolder;
	double secondsUntilBackup;
	int i;
	

	now = time(NULL);
	backupFolder = *localtime(&now);


	//Set to value to perform action at
	backupFolder.tm_hour = 13;
	backupFolder.tm_min = 55;
	backupFolder.tm_sec = 0;


	int pid = fork();
	
	if (pid>0) {
		printf("Parent process");
		sleep(10);
		exit(EXIT_FAILURE);
	} else if (pid == 0){
		printf("Child process");
		
		if (setsid() < 0) { exit(EXIT_FAILURE); }

		int pid = fork();

		if (pid > 0) {
			exit(EXIT_FAILURE);
		} else {
			umask(0);

			if (chdir("/") < 0) { exit(EXIT_FAILURE); }

			int x;
			for(x=sysconf(_SC_OPEN_MAX); x>=0; x--)
			{
				close(x);
			}

			
			//infinite loop
			while(1) {
				sleep(1);
				now = time(NULL);
				secondsUntilBackup = difftime(now,mktime(&backupFolder));
	
				if(secondsUntilBackup == 0){
					system("cp -r /home/gary/Documents/TestFolder /home/gary/Documents/backupfolder");
				}

				fd = open(fifoFile, O_RDONLY);
				read(fd, buf, MAX_BUF);
				printf("Message in: %s\n", buf);
				
				if(strncmp(buf, "Force backup", strlen("Force backup")))
				{
					system("cp -r /home/gary/Documents/TestFolder /home/gary/Documents/backupfolderNew");

				}
				close(fd);
		
			}

			
		}
	}

	return 0;
}
