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
	printf(" Wellcome %s \n---------------------------\n(Enter the empty string to back)\n",name);
	printf("Enter URL file to upload: ");
}
void signup(int clientSocket){
	char a[512],op[3],b[512];
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
	char a[255],b[1024],op[3];
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
				if (upfile(clientSocket,a)) break;
			} else {
				printf("Invalid password!\n");
				continue;
			}
		}

	}
}

int upfile(int clientSocket, char name[255]){
	char tmp[BUFF_SIZE];
	char fname[255];
	pthread_t tid;
	size_t clen=0;
	int connfd = 0,err,j=0;
	fflush(stdin);
	hello(name);
	gets(tmp);
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
	write(clientSocket, fname,256);
	FILE *fp = fopen(tmp,"rb");
	if(fp==NULL) {
		printf("File open error");
		return 0;
	}
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
			write(clientSocket,'\0',2);
			break;
		}
	}
	if (ferror(fp)) return 0;
	fclose(fp);
	return 1;
}