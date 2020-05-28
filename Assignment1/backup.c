#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int backup() {
	int pid = fork();

	if (pid == -1){
		perror("error creating fork in backup.c");
	}else if(pid == 0){

		execlp("/bin/cp","cp","-r","/var/www/html/Live_site/", "/home/gary/Documents/Backup/",NULL);
		exit(1);
	}
	return 0;

}
