#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#define BUFF_SIZE 1024

void menu()
{
	printf("\n___________________________\n");
	printf(" FILE MANAGEMENT PROGRAM \n---------------------------\n");
	printf("1. Register\n");
	printf("2. Sign in\n");
	printf("---------------------------\n");
	printf("Your choice (1 or 2, other to quit): \n");
}
void menu1(){
	printf("\n___________________________\n");
	printf(" SIGNUP \n---------------------------\n(Enter the empty string to back)\n");
	printf("Enter Username: ");
}

void menu2(){
	printf("\n___________________________\n");
	printf(" SIGNIN \n---------------------------\n(Enter the empty string to back)\n");
	printf("Enter Username: ");
}

void hello(char name[255]){
	printf("\n___________________________\n");
	printf(" Wellcome %s \n---------------------------\n",name);
}
void enter(char uname[100],cur[100]){
	printf("\n___________________________\n");
	printf("Enter the \"help\" for show command list\n");
	printf("Enter Command %s ~%s:",uname,cur);
}
void cmdlist(){
	printf("\n___________________________\n");
	printf("ls: show folder and file\nout: logout\n");
	printf("upfile [filepath]: upload file\n");
	printf("remove [filename]: delete file\n");

}
void signup(int clientSocket){
	char *a = malloc(sizeof(char)*512);
	char *op = malloc(sizeof(char)*3);
	char *b = malloc(sizeof(char)*512);
	int msg_len;
	while(1){
		memset(a,'\0',(strlen(a)+1));
		memset(b,'\0',(strlen(b)+1));
		fflush(stdin);
		menu1();
		gets(a);
		sprintf(b,"SU %s",a);
		msg_len = strlen(b);
		if (msg_len == 3){
			printf("\nBack!\n");
			break;
		}
		send(clientSocket, b, strlen(b), 0);
		memset(op,'\0',4);
		recv(clientSocket, b, BUFF_SIZE, 0);
		op[0]=b[0]; op[1]=b[1];
		if (strcmp("EX", op)==0) { printf("Account existed\n"); continue; }
		if (strcmp("OK", op)==0) { 
			fflush(stdin);
			printf("Enter password: ");
			memset(b,'\0',(strlen(b)+1));
			gets(b);
			send(clientSocket, b, strlen(b), 0);
			if (recv(clientSocket, b, BUFF_SIZE, 0)) printf("Signup sucessfuly!\n");
			break;
		}
	}
}

void signin(int clientSocket){
	char *a = malloc(sizeof(char)*255);
	char *op = malloc(sizeof(char)*3);
	char *b = malloc(sizeof(char)*1024);
	int msg_len;
	while(1){
		memset(a,'\0',(strlen(a)+1));
		memset(b,'\0',(strlen(b)+1));
		fflush(stdin);
		menu2();
		gets(a);
		sprintf(b,"SI %s",a);
		msg_len = strlen(b);
		if (msg_len == 3){
			printf("\nBack!\n");
			break;
		}
		send(clientSocket, b, strlen(b), 0);
		memset(op,'\0',4);
		recv(clientSocket, b, BUFF_SIZE, 0);
		op[0]=b[0]; op[1]=b[1];
		if (strcmp("NE", op)==0) { printf("Account not exist\n"); continue; }
		if (strcmp("OK", op)==0) { 
			fflush(stdin);
			printf("Enter password: ");
			memset(b,'\0',(strlen(b)+1));
			gets(b);
			send(clientSocket, b, strlen(b), 0);
			recv(clientSocket, b, 2, 0);
			if (b[0]=='S') {
				printf("Signin sucessfuly!\n");

				if (process(clientSocket,a)) break;	/////////////
			} else {
				printf("Invalid password!\n");
				continue;
			}
		}

	}
}

int upfile(int clientSocket, char *c1,char *c2){
	char tmp[BUFF_SIZE];
	char fname[255];
	pthread_t tid;
	size_t clen=0;
	int connfd = 0,err,j=0;
	fflush(stdin);
	strcpy(tmp,c2);
	int i=strlen(tmp)-1;
	while(1){
		if (i==0) break;
		if(tmp[i]=='/') {
			i++;
			break;
		}
		i--;
	}
	while(tmp[i]!='\0')
		fname[j++]=tmp[i++];
	printf("__%s__\n", tmp);
	printf("__%s__\n", fname);
	FILE *fp = fopen(tmp,"rb");
	if(fp==NULL) {
		printf("File open error");
		return 0;
	}
	sprintf(tmp,"%s|%s|",c1,fname);
	send(clientSocket, tmp, strlen(tmp), 0);
	while(1) {
            /* First read file in chunks of 256 bytes */
		unsigned char buff[1024]={0};
		int nread = fread(buff,1,1024,fp);
            //printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
		if(nread > 0){
                //printf("Sending \n");
			write(clientSocket, buff, nread);
		}
		if (nread < 1024) {
			if (feof(fp)) printf("End of file\n");
			if (ferror(fp))
				printf("Error reading\n");
			break;
		}
	}
	if (ferror(fp)) return 0;
	fclose(fp);
	return 1;
}
char *get1(char cmd[500]){
	char a[30];
	int i,j=0,k;
	for (i = 0; i < strlen(cmd); i++) 
		if (cmd[i] !=' ') break;
	for (k = i; k < strlen(cmd); k++){
		if (cmd[k] ==' ') break;
		a[j++]=cmd[k];
	}
	a[j]='\0';
	return &a[0];
}
char *get2(char cmd[500]){
	char b[500];
	int j=0,i,k;
	for (i = 0; i < strlen(cmd); i++) 
		if (cmd[i] !=' ') break;
	for (k = i; k < strlen(cmd); k++) 
		if (cmd[k] ==' ') break;
	for (i = k; i < strlen(cmd); i++) 
		if (cmd[i] !=' ') break;
	for (k = i; k < strlen(cmd); k++){
		if (cmd[k] ==' ') break;
		b[j++]=cmd[k];
	}
	b[j]='\0';
	return &b[0];
}

int process(int clientSocket, char uname[100]){
	char cmd[BUFF_SIZE];
	char cur[100];
	char *c1,*c2;
	hello(uname);
	while(1){
		recv(clientSocket, cur, 100, 0);
		enter(uname,cur);
		gets(cmd);
		c1 = get1(cmd);
		c2 = get2(cmd);
		if (!strcmp(c1,"out")) break;
		if (!strcmp(c1,"help")) cmdlist();
		if (!strcmp(c1,"upfile")) {
			upfile(clientSocket,c1,c2);
			continue;
		}
		if (!strcmp(c1,"downfile")){
		//downfile(clientSocket,c1,c2);
			continue;
		}
		printf("Command not found!\n");
		sprintf(cmd,"Continue");
		send(clientSocket, cmd,8,0);
	}
	return 1;
}

