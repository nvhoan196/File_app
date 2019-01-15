#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include "ServerLibrary.h"

#define BUFF_SIZE 1024
char cur[100];
char ss[BUFF_SIZE];

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
		send(newSocket, buffer, BUFF_SIZE, 0);
		recv(newSocket, buffer, BUFF_SIZE, 0);
		if (add(root,tmp,buffer)){
			MakeFolder(link,tmp);
			memset(tmp,'\0',255);
			sprintf(tmp,"S");
			send(newSocket, tmp, BUFF_SIZE, 0);
			return 0;
		}
	}else{
		memset(tmp,'\0',255);
		sprintf(tmp,"EX");
		send(newSocket, tmp, BUFF_SIZE, 0);
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
		send(newSocket, tmp, BUFF_SIZE, 0);
		return 1;
	}else{
		memset(buffer,'\0',(strlen(buffer)+1));
		sprintf(buffer,"OK");
		send(newSocket, buffer, BUFF_SIZE, 0);
		recv(newSocket, buffer, BUFF_SIZE, 0);
		if (checkpass(root,tmp,buffer)){
			memset(buffer,'\0',255);
			sprintf(buffer,"SS");
			send(newSocket, buffer, BUFF_SIZE, 0);
			process(newSocket, tmp);
		} else {
			memset(tmp,'\0',255);
			sprintf(tmp,"CC");
			send(newSocket, tmp, BUFF_SIZE, 0);
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
	char fname[255],linkf[BUFF_SIZE];
	strcpy(linkf,link);
	char recvBuff[1024];
	int bytesReceived = 0;
	int i,j=0,k,l;
	l=strlen(linkf);
	linkf[l]='/';
	for (i = 0; i < strlen(buffer); i++) if (buffer[i] =='|') break;
	for (k = i+1; k < strlen(buffer); k++){
		if (buffer[k] =='|') break;
		linkf[++l]=buffer[k];
		fname[j++]=buffer[k];
	}
	linkf[++l]='\0';
	printf("File Name: %s\n",fname);
	printf("Receiving file...");
	FILE *fp = fopen(linkf, "ab"); 
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

int remo(int sockfd,char buffer[BUFF_SIZE], char link[BUFF_SIZE]){
	char linkf[BUFF_SIZE];
	char sys[BUFF_SIZE];
	int i,l,k;
	strcpy(linkf,link);
	l=strlen(linkf);
	linkf[l]='/';
	for (i = 0; i < strlen(buffer); i++) if (buffer[i] =='|') break;
	for (k = i+1; k < strlen(buffer); k++){
		if (buffer[k] =='|') break;
		linkf[++l]=buffer[k];
	}
	linkf[++l]='\0';
	sprintf(sys,"rm -rf %s", linkf);
	printf("===%s===\n", sys);
	if (!system(sys)) {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"OK");
		send(sockfd,buffer,BUFF_SIZE,0);
		return 1;
	}
	else {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"ER");
		send(sockfd,buffer,BUFF_SIZE,0);
		return 0;
	}
}

int dete(int sockfd,char buffer[BUFF_SIZE], char link[BUFF_SIZE]){
	char linkf[BUFF_SIZE];
	int i,l,k;
	strcpy(linkf,link);
	l=strlen(linkf);
	linkf[l]='/';
	for (i = 0; i < strlen(buffer); i++) if (buffer[i] =='|') break;
	for (k = i+1; k < strlen(buffer); k++){
		if (buffer[k] =='|') break;
		linkf[++l]=buffer[k];
	}
	linkf[++l]='\0';
	if (!remove(linkf)) {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"OK");
		send(sockfd,buffer,BUFF_SIZE,0);
		return 1;
	}
	else {
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"ER");
		send(sockfd,buffer,BUFF_SIZE,0);
		return 0;
	}
}
int go(int sockfd,char buffer[BUFF_SIZE], char link[BUFF_SIZE]){
	DIR *dir;
	char meg[BUFF_SIZE];
	struct dirent *ent;
	char fname[255],linkf[BUFF_SIZE];
	strcpy(linkf,link);
	int i,j=0,k,l;
	l=strlen(linkf);
	linkf[l]='/';
	for (i = 0; i < strlen(buffer); i++) if (buffer[i] =='|') break;
	for (k = i+1; k < strlen(buffer); k++){
		if (buffer[k] =='|') break;
		linkf[++l]=buffer[k];
		fname[j++]=buffer[k];
	}
	linkf[++l]='\0';
	fname[j]='\0';
	if ((dir = opendir(linkf)) != NULL) {
  /* print all the files and directories within directory */
		bzero(meg, sizeof(meg));
		sprintf (meg,"OK");
		send(sockfd,meg,BUFF_SIZE,0);
		sprintf (ss,"%s",linkf);
		sprintf(cur,"%s",fname);
		closedir (dir);
	} else {
  /* could not open directory */
		bzero(meg, sizeof(meg));
		sprintf (meg,"ER");
		send(sockfd,meg,BUFF_SIZE,0);
		perror ("");
		return 1;
	}
}
int back(int sockfd,char buffer[BUFF_SIZE], char link[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	int i,k,j=0;
	for (i = strlen(link); i > 3; i--) if (link[i] =='/') break;
	printf("%d\n", i);
	for (k = i; k < strlen(link); k++) link[k]='\0';
	for (i = strlen(link); i > 3; i--) if (link[i] =='/') break;
	for (k = i; k < strlen(link); k++) cur[j++]=link[k];
	cur[j]='\0';
	if (!strcmp("SD/",link)) {
		bzero(meg, sizeof(meg));
		sprintf (meg,"ER");
		send(sockfd,meg,BUFF_SIZE,0);
		return 0;
	}
	sprintf (ss,"%s",link);
	bzero(meg, sizeof(meg));
	sprintf (meg,"OK");
	send(sockfd,meg,BUFF_SIZE,0);
	return 1;
}
void ls(int sockfd,char buffer[BUFF_SIZE], char link[BUFF_SIZE]){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(link)) != NULL) {
  	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			bzero(buffer, sizeof(buffer));
			sprintf (buffer,"%s", ent->d_name);
			send(sockfd,buffer,BUFF_SIZE,0);
		}
		bzero(buffer, sizeof(buffer));
		sprintf (buffer,"  ");
		send(sockfd,buffer,BUFF_SIZE,0);
		printf("DA GUI TH KET THUC\n");
		closedir (dir);
	} else {
  	/* could not open directory */
		perror ("");
	}
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
    	upfile(newSocket,buffer,link);
    	return 1;
    }
    if (!strcmp(p,"ls")){
    	ls(newSocket,buffer,link);
    	return 1;
    }
    if (!strcmp(p,"delete")){
    	dete(newSocket,buffer,link);
       	return 1;
    }
    if (!strcmp(p,"remove")){
    	remo(newSocket,buffer,link);
       	return 1;
    }
    if (!strcmp(p,"goto")){
    	go(newSocket,buffer,link);
       	return 1;
    }
    if (!strcmp(p,"back")){
    	back(newSocket,buffer,link);
       	return 1;
    }
    return 0;
}

int process(int sockfd, char uname[100]){
	char link[BUFF_SIZE]; ///vitri
	char buffer[BUFF_SIZE];
	int j=3;
	strcpy(link,"SD/");
	int i;
	strcat(link,uname);
	strcpy(cur,uname);
	strcpy(ss,link);
	while(1){
		printf("Cur:==%s==\n", cur);
		send(sockfd, cur, BUFF_SIZE, 0);
		bzero(buffer, sizeof(buffer));
		strcpy(link,ss);
		printf("%s\n", link);
		recv(sockfd, buffer, BUFF_SIZE, 0);
		printf("\nBUFF---%s---\n", buffer);	//////////////////
		if (!strcmp(buffer,"Continue")) continue;
		if (!strcmp(buffer,"out")) break;
		if (buffer[0]=='\0') break;
		if(checkOpCode(sockfd, buffer, link) == 0) {
			strcpy(buffer, "FUNCTION_NOT_EXIST");
            printf("%s\n", buffer);
            send(sockfd, buffer, BUFF_SIZE, 0);
        }
	}
	return 1;
}