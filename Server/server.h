#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include "ServerLibrary.h"

#define BUFF_SIZE 1024


typedef struct Account
{
	char name[100];
	char pass[100];
}Account;
typedef struct Alist
{
	struct Alist *next;
	Account acc;
}Alist;

void update(Alist *root){			//update account data
	FILE *f;
	char filename[] = "account.txt";
	if ((f = fopen(filename, "w")) == NULL) 
		printf("Can not open file %s!\n",filename);
	else {
		Alist *cur;
		cur=root;
		while(cur->next!=NULL){
			cur=cur->next;
			fprintf(f, "%s ",(cur->acc).name);
			fprintf(f, "%s",(cur->acc).pass);
			if(cur->next!=NULL) fprintf(f, "\n");
		}
	}
	fclose(f);
}
void readfile(Alist *root){			//Read account data
	char s[255];
	int i;
	Alist *cur;
	cur=root;
	FILE *f;
	char filename[] = "account.txt";
	if ((f = fopen(filename, "r")) == NULL){
		printf("Can not open file %s!\n",filename);
	} else{
		while (1){
			Alist *new;
			new=(Alist*)malloc(sizeof(Alist));
			fscanf(f,"%s", (new->acc).name);
			fscanf(f,"%s", (new->acc).pass);
			memset(s,'\0',strlen(s));
			new->next=NULL;
			cur->next=new;
			cur=new;
			if(feof(f)) break;
		}
		fclose(f);
	}
}
int checkname(char tmp[255], Alist *root){
	Alist *cur;
	cur=root;
	while(cur->next!=NULL){
		cur=cur->next;
		if (strcmp((cur->acc).name, tmp)==0) return 0;
	}
	return 1;
}
int signup(int newSocket,Alist *root,char tmp[255]){
	char link[]="SD";
	char buffer[512];
	if (checkname(tmp,root)) {
		memset(buffer,'\0',(strlen(buffer)+1));
		sprintf(buffer,"OK");
		send(newSocket, buffer, 2, 0);
		recv(newSocket, buffer, BUFF_SIZE, 0);
		if (add(root,tmp,buffer)){
			MakeFolder(link,tmp);
			memset(tmp,'\0',255);
			sprintf(tmp,"S");
			send(newSocket, tmp, 1, 0);
			return 0;
		}
	}else{
		memset(tmp,'\0',255);
		sprintf(tmp,"EX");
		send(newSocket, tmp, strlen(tmp), 0);
		return 1;
	}
}

int add(Alist *root,char name[255],char pass[255]){
	Alist *new;
	Alist *cur;
	cur=root;
	new=(Alist*)malloc(sizeof(Alist));
	strcpy((new->acc).name,name);
	strcpy((new->acc).pass,pass);
	new->next=NULL;
	while(cur->next!=NULL) cur=cur->next;
	cur->next=new;
	update(root);
	return 1;
}

int signin(int newSocket,Alist *root,char tmp[255]){
	int buffer[255];
	printf("%s\n", tmp);
	if (checkname(tmp,root)) {
		memset(tmp,'\0',255);
		sprintf(tmp,"NE");
		send(newSocket, tmp, strlen(tmp), 0);
		return 1;
	}else{
		memset(buffer,'\0',(strlen(buffer)+1));
		sprintf(buffer,"OK");
		send(newSocket, buffer, 2, 0);
		recv(newSocket, buffer, BUFF_SIZE, 0);
		if (checkpass(root,tmp,buffer)==1){
			memset(buffer,'\0',255);
			sprintf(buffer,"S");
			send(newSocket, buffer, 1, 0);
			process(newSocket, tmp);
		} else {
			memset(tmp,'\0',255);
			sprintf(tmp,"C");
			send(newSocket, tmp, 1, 0);
		}
	}
	return 0;
}

int checkpass(Alist *root,char name[255],char pass[255]){
	Alist *cur;
	cur=root;
	while(cur->next!=NULL){
		cur=cur->next;
		if (strcmp((cur->acc).name, name)==0) break;
	}
	if (strcmp((cur->acc).pass, pass)==0) return 1;
	return 0;
}

int upfile(int sockfd,char buffer[BUFF_SIZE],char link[BUFF_SIZE]){
	char fname[255];
	char recvBuff[1024];
	int bytesReceived = 0;
	int i,j=0,k,l;
	l=strlen(link);
	link[l]='/';
	for (i = 0; i < strlen(buffer); i++) if (buffer[i] =='|') break;
	for (k = i+1; k < strlen(buffer); k++){
		if (buffer[k] =='|') break;
		link[++l]=buffer[k];
		fname[j++]=buffer[k];
	}
	link[++l]='\0';
	printf("File Name: ++%s++\n",fname);
	printf("LINK++%s++++\n", link);
	printf("Receiving file...");
	FILE *fp = fopen(link, "ab"); 
	if(NULL == fp){
		printf("Error opening file");
		return 0;
	}
	long double sz=1;
    /* Receive data in chunks of 256 bytes */
	while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
		sz++;
		fflush(stdout);
		fwrite(recvBuff, 1,bytesReceived,fp);
		if (bytesReceived < 1024) break;
	}
	printf("Received: %llf Mb",(sz/1024));

	if(bytesReceived < 0) {
		printf("\n Read Error \n");
		return 0;
	}
	printf("\nFile OK....Completed\n");
	fclose(fp);
	return 1;
}
int checkOpCode(int newSocket, char buffer[BUFF_SIZE],char link[BUFF_SIZE]){
    char *p;
    char str[BUFF_SIZE];
    strcpy(str, buffer);
    p = strtok(str,"|");
    if(strcmp(p, "downfile")==0){
    	//downfile(newSocket, buffer); //downfile server
    	return 1;
    }
    if (!strcmp(p,"upfile")){
    	upfile(newSocket, buffer,link);
    	return 1;
    }
    return 0;
}

int process(int sockfd, char uname[100]){
	char link[BUFF_SIZE]; ///vitri
	char buffer[BUFF_SIZE];
	char cur[100];
	int j=3;
	strcpy(link,"SD/");
	int i;
	strcat(link,uname);
	strcpy(cur,uname);
	while(1){
		send(sockfd, cur, strlen(cur), 0);
		bzero(buffer, sizeof(buffer));
		recv(sockfd, buffer, BUFF_SIZE, 0);
		printf("BUFF---%s---\n", buffer);	//////////////////
		if (!strcmp(buffer,"Continue")) continue;
		if(buffer[0] == '\0'){
			break;
		}else checkOpCode(sockfd, buffer,link);
	}
	return 1;
}