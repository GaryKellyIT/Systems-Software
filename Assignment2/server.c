#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>

//Global
pthread_mutex_t lock;
int thread_count = 0;

int receiveAcknowledge(int cs,char *location, char *message){
	char clientMessage[500];
	int READSIZE;
	memset(clientMessage,0,500);
	READSIZE = recv(cs, clientMessage, sizeof(clientMessage), 0);
	if(READSIZE == 0){
		printf("Client disconnected\n");
		return 1;
	} else if(READSIZE == -1){
		printf("Read error\n");
		return 1;
	}
	strcpy(location, clientMessage);
	write(cs, message, strlen(message));
	return 0;
}


void *file_transfer(void *csocket){
	int i, serverUID, clientUID, return_status,ngroups;
	uid_t uid,gid,euid,egid;
	int cs = *(int *)csocket;
	char filepath[500];
	char filecontents[500];
	char clientUIDstring[500];
	char message[500];
	gid_t supp_groups[] = {};
	gid_t *groups;
	struct passwd *pw;
	struct group *gr;
	FILE *fp;

	printf("\nThread ID : %d\n",pthread_self());
	
	
	//read in filename
	memset(message,0,500);
	memset(filepath,0,500);
	strcpy(message,"Filename Received");
	return_status = receiveAcknowledge(cs, filepath, message);
	if(return_status == 0)
	{
		printf("\nFilepath is : %s",filepath);
	} else{
		perror("Error reading filepath");
		return NULL;
	}

	//read in file contents
	memset(message,0,500);
	memset(filecontents,0,500);
	strcpy(message,"Filecontents Received");
	return_status = receiveAcknowledge(cs, filecontents, message);
	if(return_status == 0)
	{
		printf("\nFilecontents are : %s",filecontents);
	} else{
		perror("Error reading filecontents");

		memset(message,0,500);
		strcpy(message,"File not transfered -> Error reading filecontents");
		write(cs,message,sizeof(message));
		return NULL;
	}

	//read in uid
	memset(message,0,500);
	memset(clientUIDstring,0,500);
	strcpy(message,"UID received");
	return_status = receiveAcknowledge(cs, clientUIDstring, message);
	if(return_status == 0)
	{
		printf("\nClient UID is : %s",clientUIDstring);
	} else{
		perror("Error reading UID");
		return NULL;
	}
		
	clientUID = atoi(clientUIDstring);
	serverUID = geteuid();
	//printf("\nServer UID is : %d\nClient UID is : %d",serverUID,clientUID);

	ngroups = 15;
	groups = malloc(ngroups * sizeof(gid_t));
	pw = getpwuid(clientUID);
	if (pw == NULL)
	{
		perror("\nError with getpwuid");
		memset(message,0,500);
		strcpy(message,"File not transfered -> Error with getpwuid");
		write(cs,message,sizeof(message));
		return NULL;
	}

	if(getgrouplist(pw->pw_name,pw->pw_gid,groups,&ngroups) == -1) {
		perror("\nError with getgrouplist");
		memset(message,0,500);
		strcpy(message,"File not transfered -> Error writh getgrouplist");
		write(cs,message,sizeof(message));
		return NULL;
	}

	printf("\nClient groups are : ");
	for (int j = 0;j<ngroups;j++)
	{
		supp_groups[j] = groups[j];
		printf(" - %d",supp_groups[j]);
	}

	uid = getuid();
	gid = getgid();
	euid = geteuid();
	egid = getegid();

	printf("\n\nBefore change to client\n");
	printf("UID is : %d\n",uid);
	printf("GID is : %d\n",gid);
	printf("EUID is : %d\n",euid);
	printf("EGID is : %d\n",egid);


	setgroups(15,supp_groups);
	setreuid(clientUID,uid);
	setregid(clientUID,gid);
	seteuid(clientUID);
	setegid(clientUID);


	printf("\nAfter change to client\n");
	printf("UID is : %d\n",getuid());
	printf("GID is : %d\n",getgid());
	printf("EUID is : %d\n",geteuid());
	printf("EGID is : %d\n",getegid());

	printf("\nMutex locking to ensure thread synchronisation\n");

	pthread_mutex_lock(&lock);
	fp = fopen(filepath,"w");
	
	if( fp != NULL)
	{
		fwrite(filecontents,sizeof(char),sizeof(filecontents), fp);
		printf("File successfully transfered\n");
		
	} else {
		perror("Error writing to file");

		memset(message,0,500);
		strcpy(message,"File not transfered -> Error writing to file");
		write(cs,message,sizeof(message));

		serverUID = 0;
		setuid(serverUID);
		setreuid(serverUID,uid);
		setregid(serverUID,gid);
		seteuid(serverUID);
		setegid(serverUID);

		printf("Mutex unlocking\n");
		pthread_mutex_unlock(&lock);

		pthread_exit(NULL);
	}

	printf("Mutex unlocking\n");

	fclose(fp);
	pthread_mutex_unlock(&lock);
	serverUID = 0;
	setuid(serverUID);
	setreuid(serverUID,uid);
	setregid(serverUID,gid);
	seteuid(serverUID);
	setegid(serverUID);

	printf("\nAfter change back to server\n");
	printf("UID is : %d\n",getuid());
	printf("GID is : %d\n",getgid());
	printf("EUID is : %d\n",geteuid());
	printf("EGID is : %d\n",getegid());

	memset(message,0,500);
	strcpy(message,"File transfered");
	write(cs,message,sizeof(message));
	printf("\nThread ID : %d closed\n",pthread_self());
	pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
	int s;
	int cs;
	int connSize;
	

	struct sockaddr_in server , client;
	char message[500];

	s = socket(AF_INET , SOCK_STREAM, 0);
	if (s==-1)
	{
		printf("Could not create socket\n");
	} else {
		printf("\nSocket Successfully Created\n");
	}

	server.sin_port = htons( 8081 );
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;


	if( bind(s,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Bind issue\n");
		return 1;
	} else {
		printf("Bind complete\n");
	}

	listen(s,3);

	printf("<<Waiting for incoming connection from Client>>\n");
	connSize = sizeof(struct sockaddr_in);

	if(pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed");
		return 1;
	}

	while(cs = accept(s, (struct sockaddr *)&client, (socklen_t*)&connSize))
	{
		pthread_t thread;
		int *client_socket = malloc(200);
		*client_socket = cs;
	
		if(pthread_create( &thread,NULL, file_transfer, (void*)client_socket) < 0){	
			perror("Couldn't create thread");
		} else{
			printf("\nJob %d created\n",thread_count);
			thread_count = thread_count + 1;
		}
	}

	pthread_mutex_destroy(&lock);
	return 0;
}
