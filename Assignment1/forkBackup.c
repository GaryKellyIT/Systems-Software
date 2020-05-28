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
#include <mqueue.h>
#include "allowAccess.h"
#include "denyAccess.h"
#include "backup.h"
#include "transfer.h"
//#include  "generateLogs.h"

#define MAX_BUF 1024

int main()
{
	char *livedir = "/var/www/html/Live_site";
	char *backupdir = "/home/gary/Documents/Backup";
	char *intranetdir = "/var/www/html/Intranet";

	int fd;
	mqd_t mq;
   	struct mq_attr queue_attributes;
    	char buffer[1024 + 1];
    	int terminate = 0;


	/* set queue attributes */
	queue_attributes.mq_flags = 0;
	queue_attributes.mq_maxmsg = 10;
    	queue_attributes.mq_msgsize = 1024;
    	queue_attributes.mq_curmsgs = 0;

	
	
	
	time_t now;
	struct tm backupFolder;
	double secondsUntilBackup;
	int i;
	

	now = time(NULL);
	backupFolder = *localtime(&now);


	//Set to value to perform action at
	backupFolder.tm_hour = 11;
	backupFolder.tm_min = 34;
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

			int pid_2 = fork();
			if  (pid_2 == 0){
				//Message queue to force backup through client.c
				while(1) {
					/* create queue */
	    				mq = mq_open("/dt228_queue", O_CREAT | O_RDONLY, 0644, &queue_attributes);
					
					ssize_t bytes_read;

					/* receive message */
					bytes_read = mq_receive(mq, buffer, 1024, NULL);

					buffer[bytes_read] = '\0';
					if (! strncmp(buffer, "backup", strlen("backup")))
					{ 
						int forced_backup_result = backup();
					}
					else if (! strncmp(buffer, "update", strlen("update")))
					{
						int access_result  = allowAccess();
						if(access_result == 0){
							syslog(LOG_INFO,"Successfully opened access to Live_site");
						}else{
							syslog(LOG_INFO,"Failed to open access to Live_site");
						}

						int backup_result = backup();
						if(backup_result == 0){
							syslog(LOG_INFO,"Successfully backed up Live_site to  /home/gary/Documents/Backup");
						}else{
							syslog(LOG_INFO,"Failed to back up Live_site to  /home/gary/Documents/Backup");
						}

						int transfer_result = transfer();
						if(transfer_result == 0){
							syslog(LOG_INFO,"Successfully transferred changed files from Intranet to Live_site");
						}else{
							syslog(LOG_INFO,"Failed to transfer changed files from Intranet to Live_site");
						}

						int deny_access_result  = denyAccess();
						if(deny_access_result == 0){
							syslog(LOG_INFO,"Successfully closed access to Live_site");
						}else{
							syslog(LOG_INFO,"Failed to close access to Live_site");
						}
					}
					else
					{ syslog(LOG_INFO,"Received: %s\n", buffer); }

				}
			}

			//infinite loop
			while(1) {
				sleep(1);
				now = time(NULL);
				secondsUntilBackup = difftime(now,mktime(&backupFolder));
				
	
				if(secondsUntilBackup == 0){
					openlog("Logs",LOG_PID,LOG_USER);
					
					int access_result  = allowAccess();
					if(access_result == 0){
						syslog(LOG_INFO,"Successfully opened access to Live_site");
					}else{
						syslog(LOG_INFO,"Failed to open access to Live_site");
					}
					/*
					int ausearch_result = generateLogs();
					if(ausearch_result == 0) {
						syslog(LOG_INFO,"Successfully generated logs to  /home/gary/Documentss/auditlogs.txt");
					}else{
						syslog(LOG_INFO,"Failed to generate logs to  /home/gary/Documentss/auditlogs.txt");
					}*/
					
					int backup_result = backup();
					if(backup_result == 0){
						syslog(LOG_INFO,"Successfully backed up Live_site to  /home/gary/Documents/Backup");
					}else{
						syslog(LOG_INFO,"Failed to back up Live_site to  /home/gary/Documents/Backup");
					}

					int transfer_result = transfer();
					if(transfer_result == 0){
						syslog(LOG_INFO,"Successfully transferred changed files from Intranet to Live_site");
					}else{
						syslog(LOG_INFO,"Failed to transfer changed files from Intranet to Live_site");
					}

					int deny_access_result  = denyAccess();
					if(deny_access_result == 0){
						syslog(LOG_INFO,"Successfully closed access to Live_site");
					}else{
						syslog(LOG_INFO,"Failed to close access to Live_site");
					}
					
						
					closelog();

				}
				
			}
			
			mq_close(mq);
			mq_unlink("/dt228_queue");
			
		}
	}

	return 0;
}
