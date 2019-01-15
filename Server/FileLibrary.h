#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

// return 0 if success
// return -1 if error
int RecvFile(int sockfd, const char *filePath){
	FILE *fl;
	int fileSize, nLeft;
	char ch;

	fl = fopen(filePath,"w+");
	if(fl == NULL){
		perror("Error");
		return -1;
	}

	recv(sockfd, &fileSize,sizeof(fileSize),0);
	printf("%d\n",fileSize);
	nLeft = fileSize;
	while(nLeft > 0){
		recv(sockfd, &ch, sizeof(char), 0);
		printf("%c\n",ch);
		fputc(ch,fl);
		nLeft--;
	}
	fclose(fl);
	return 0;
}

// return 0 if success
// return -1 if error
int CheckFileExist(const char*filePath)
{
	FILE *fl;
	fl = fopen(filePath,"w");
	if(fl == NULL){
		return -1;
	}
	return 0;
}

// return 0 if success
// return -1 if error
int RemoveFile(const char *filePath){
	return remove(filePath);
}

// return 0 if success
// return -1 if error
int Move_File(const char *filePath, const char *newPath){
	FILE *fl_Old;
	FILE *fl_New;
	char ch;
	fl_Old = fopen(filePath,"r");
	fl_New = fopen(newPath,"w+");
	if(fl_Old == NULL || fl_New == NULL){
		return -1;
	}
	while((ch = fgetc(fl_Old)) != EOF){
		fputc(ch,fl_New);
	}
	fclose(fl_Old);
	fclose(fl_New);
	RemoveFile(filePath);
	return 0;
}