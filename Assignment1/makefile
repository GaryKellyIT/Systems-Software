myprog : daemon.o allowAccess.o denyAccess.o backup.o transfer.o generateLogs.o
	gcc -o daemon daemon.c allowAccess.o denyAccess.o backup.o transfer.o generateLogs.o -lrt

daemon.o : daemon.c allowAccess.h denyAccess.h backup.h transfer.h generateLogs.h
	gcc -c daemon.c -lrt

allowAccess.o :  allowAccess.c
	gcc -c  allowAccess.c

denyAccess.o : denyAccess.c 
	gcc -c denyAccess.c

backup.o : backup.c
	gcc -c backup.c

transfer.o : transfer.c
	gcc -c transfer.c

generateLogs.o : generateLogs.c
	gcc -c generateLogs.c

clean:
	rm daemon daemon.o allowAccess.o denyAccess.o backup.o transfer.o generateLogs.o
