#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int sendAcknowledge(int SID,char *location){
	char serverMessage[500];
	//send filename
	if( send(SID, location, strlen(location) , 0) < 0)
	{
		perror("Error sending file\n");
		return 1;
	} 

	//receive filename confirmation from server
	memset(serverMessage,0,sizeof(serverMessage));
	recv(SID,serverMessage,sizeof(serverMessage),0);
	printf("\nServer Message : %s",serverMessage);
	return 0;
}

int main (int argc, char *argv[])
{
	int SID,b, return_status;
	struct sockaddr_in server;
	char filename[500];
	char serverMessage[500];
	char message[500];
	char filepath[500];
	char filecontents[500];
	int option;
	FILE *fp;

	char root[]= "/var/www/html/Assignment2/Intranet/";
	char sales[] = "/var/www/html/Assignment2/Intranet/Sales/";
	char marketing[] = "/var/www/html/Assignment2/Intranet/Marketing/";
	char offers[] = "/var/www/html/Assignment2/Intranet/Offers/";
	char promotions[] = "/var/www/html/Assignment2/Intranet/Promotions/";

	SID = socket(AF_INET , SOCK_STREAM , 0);
	if (SID == -1)
	{
		printf("Error creating socket");
	} else{
		printf("socket created\n");
	}

	server.sin_port = htons( 8081 );
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;

	if (connect(SID, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("conect failed. Error\n");
		return 1;
	}

	printf("Connected to server ok\n");
	option = 0;
	printf("\nEnter filename : ");
	scanf("%s", filename);

	bzero(filepath,sizeof(filepath));
	do
	{
		printf("Choose destination directory\n1)Root\n2)Sales\n3)Marketing\n4)Offers\n5)Promotions\n6)Exit\n\n->");
		scanf("%d", &option);


		switch(option)
		{
			case 1 :
				strcat(root,filename);
				strcpy(filepath,root);
				printf("\nfilepath is %s\n",filepath);
				break;

			case 2 :
				strcat(sales,filename);
				strcpy(filepath,sales);
				printf("\nfilepath is %s\n",filepath);
				break;

			case 3 :
				strcat(marketing,filename);
				strcpy(filepath,marketing);
				printf("\nfilepath is %s\n",filepath);
				break;

			case 4 :
				strcat(offers,filename);
				strcpy(filepath,offers);
				printf("\nfilepath is %s\n",filepath);
				break;

			case 5 :
				strcat(promotions,filename);
				strcpy(filepath,promotions);
				printf("\nfilepath is %s\n",filepath);
				break;

			case 6 :
				printf("\nClient terminated connection\n");
				exit(0);

			default :
				printf("\nIncorrect option chosen\n");
				break;

		}
		
	} while((option != 1) && (option != 2) && (option != 3) && (option != 4) && (option != 5));

	fp = fopen(filename,"r");
	if(fp == NULL)
	{
		perror("Error opening file\n");
		return 1;
	}	

	printf("\nSending filename");
	return_status = sendAcknowledge(SID,filepath);
	if(return_status != 0)
	{
		printf("Error sending filename");
		return 1;
	}
	
	memset(filecontents,0,sizeof(filecontents));
	fread(filecontents,sizeof(char),sizeof(filecontents),fp);


	printf("\nSending filecontents");
	return_status = sendAcknowledge(SID,filecontents);
	if(return_status != 0)
	{
		printf("Error sending filecontents");
		return 1;
	}

	
	uid_t uid = geteuid();
	gid_t gid = getegid();
	char tmp[10];
	sprintf(tmp,"%d",uid);

	printf("\nSending UID");
	return_status = sendAcknowledge(SID,tmp);
	if(return_status != 0)
	{
		printf("Error sending UID");
		return 1;
	}


	printf("\n\nThis process is associated with UID : %d and GID : %d\n",uid,gid);

	memset(serverMessage,0,sizeof(serverMessage));
	recv(SID,serverMessage,sizeof(serverMessage),0);
	printf("\nServer Message : %s\n",serverMessage);
		

	fclose(fp);
	close(SID);

	printf("Client connection closed\n\n");
	return 0;
}

