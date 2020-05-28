#include <stdio.h>
#include <stdlib.h>

int allowAccess() {
	/*const char *filename = argv[1];
	const char *mode = argv[2];
	*/	
	char readMode[] = "0000";
	char writeMode[] = "0000";
	char *intranetdir = "/var/www/html/Intranet";
	char *livedir = "/var/www/html/Live_site";
	int i;



	
	i = strtol(readMode,0,8);
	printf("i is %d",i);
	if(chmod (intranetdir,i) < 0)
	{
		exit(1); //ERROR
	}

	i = strtol(writeMode,0,8);
	printf("i is %d",i);
	if(chmod (livedir,i) < 0)
	{
		exit(1); //ERROR
	}
	return 0;
}
