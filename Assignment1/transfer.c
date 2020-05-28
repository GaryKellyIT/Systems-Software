#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int transfer() {
	int pid = fork();

	if (pid == -1){
		perror("error creating fork in transfer.c");
	}else if(pid == 0){

		execlp("/bin/rsync","rsync","-r","/var/www/html/Intranet/","/var/www/html/Live_site/",NULL);
		//system("rsync -r /var/www/html/Intranet/ /var/www/html/Live_site/");
		exit(1);
	}
	return 0;

}
