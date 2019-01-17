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
char cur[100];

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
void cmdlist(int clientSocket){
	char meg[]="Continue";
	send(clientSocket, meg,BUFF_SIZE,0);
	printf("\n___________________________\n");
	printf("ls: show folder and file\nout: logout\n");
	printf("upfile|[file_path]: upload file\n");
	printf("delete|[file_name]: delete file\n");
	printf("remove|[folder_name]: delete folder\n");
	printf("back: Go back to the parent folder\n");
	printf("home: Go back to home folder\n");

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
		send(clientSocket, b, BUFF_SIZE, 0);
		memset(op,'\0',4);
		recv(clientSocket, b, BUFF_SIZE, 0);
		op[0]=b[0]; op[1]=b[1];
		if (strcmp("EX", op)==0) { printf("Account existed\n"); continue; }
		if (strcmp("OK", op)==0) { 
			fflush(stdin);
			printf("Enter password: ");
			memset(b,'\0',(strlen(b)+1));
			gets(b);
			send(clientSocket, b, BUFF_SIZE, 0);
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
		send(clientSocket, b, BUFF_SIZE, 0);
		memset(op,'\0',4);
		recv(clientSocket, b, BUFF_SIZE, 0);
		op[0]=b[0]; op[1]=b[1];
		if (strcmp("NE", op)==0) { printf("Account not exist\n"); continue; }
		if (strcmp("OK", op)==0) { 
			fflush(stdin);
			printf("Enter password: ");
			memset(b,'\0',(strlen(b)+1));
			gets(b);
			send(clientSocket, b, BUFF_SIZE, 0);
			recv(clientSocket, b, BUFF_SIZE, 0);
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
	FILE *fp;
	if((fp= fopen(tmp,"rb"))==NULL) {
		printf("File open error");
		return 0;
	}
	sprintf(tmp,"%s|%s|",c1,fname);
	send(clientSocket, tmp, BUFF_SIZE, 0);
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
		if (cmd[k] =='|') break;
		a[j++]=cmd[k];
	}
	a[j]='\0';
	return &a[0];
}
char *get2(char cmd[500]){
	char b[500];
	int j=0,i,k;
	for (i=0; i < strlen(cmd); i++) 
		if (cmd[i] =='|') break;
	for (k = i+1; k < strlen(cmd); k++){
		b[j++]=cmd[k];
	}
	b[j]='\0';
	return &b[0];
}
void conti(int clientSocket){
	send(clientSocket, "Continue",BUFF_SIZE,0);
}
int remo(int clientSocket,char c1[8],char c2[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	sprintf(meg,"%s|%s|",c1,c2);
	send(clientSocket,meg,BUFF_SIZE,0);
	memset(meg,'\0',(strlen(meg)+1));
	recv(clientSocket,meg,BUFF_SIZE,0);
	if (!strcmp(meg,"OK")) printf("Delete sucessfuly!\n");
	else printf("Folder \"%s\" not exist!\n", c2);
	return 1;
}
int dele(int clientSocket,char c1[8],char c2[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	sprintf(meg,"%s|%s|",c1,c2);
	send(clientSocket,meg,BUFF_SIZE,0);
	memset(meg,'\0',(strlen(meg)+1));
	recv(clientSocket,meg,BUFF_SIZE,0);
	if (!strcmp(meg,"OK")) printf("Delete sucessfuly!\n");
	else printf("File \"%s\" not exist!\n", c2);
	return 1;
}
void ls(int clientSocket,char c1[3],char c2[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	sprintf(meg,"%s|%s|",c1,c2);
	send(clientSocket,meg,BUFF_SIZE,0);
	while(1){
		bzero(meg, sizeof(meg));
		recv(clientSocket,meg,BUFF_SIZE,0);
		if (meg[0]==' ') break;
		printf("%s\n", meg);
	}
}
int go(int clientSocket,char c1[10],char c2[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	sprintf(meg,"%s|%s|",c1,c2);
	send(clientSocket,meg,BUFF_SIZE,0);
	memset(meg,'\0',(strlen(meg)+1));
	recv(clientSocket,meg,BUFF_SIZE,0);
	if (!strcmp(meg,"OK")) printf("\n");
	else printf("Can not goto folder \"%s\"\n", c2);
	return 1;
}
int back(int clientSocket,char c1[10],char c2[BUFF_SIZE]){
	char meg[BUFF_SIZE];
	sprintf(meg,"%s|%s|",c1,c2);
	send(clientSocket,meg,BUFF_SIZE,0);
	memset(meg,'\0',(strlen(meg)+1));
	recv(clientSocket,meg,BUFF_SIZE,0);
	if (!strcmp(meg,"OK")) printf("\n");
	else printf("Can not to back folder %s\n", c2);
	return 1;
	}

	//downfile_client.h

	void downFile(int clientSocket, char buffer[1024])
    {
        FILE *f1;

        char *p;
        char str[1024];
        char *fileName = (char *)malloc(255);
        send(clientSocket, buffer, BUFF_SIZE, 0);
        printf("%s\n", buffer);
        strcpy(str, buffer);
    //example : DOWNFILE|test.txt|/Users/mavuong/Desktop|
        p = strtok(str, "|");
        p = strtok(NULL, "|");
        strcpy(fileName, p);
        p = strtok(NULL, "|");//file path
        strcat(p, "/");
        strcat(p, fileName);
        printf("%s\n", p);
        f1 = fopen(p,"wb");
            if(f1 == NULL) {
                                        printf("Error! Invalid input file\n");
                                        exit(1);
                                    }
            while(1) {
            if(recv(clientSocket, buffer, 1024, 0) < 0){
            			printf("[-]Error in receiving data.\n");
            			break;
            		}else{
            			if(strcmp(buffer, "endfile") == 0){
            			printf("****File download success!****\n");
            			break;
            			}
                        fprintf(f1,"%s",buffer);
            		}
            }
    		fclose(f1);
    }

    //end downfile_client/h


    void renameClient(int clientSocket, char buffer[1024])
    {

                send(clientSocket, buffer, BUFF_SIZE, 0);
                bzero(buffer, sizeof(buffer));
                recv(clientSocket, buffer, 1024, 0);
                if(strcmp(buffer, "OK") == 0) {
                    printf("rename success!\n");
                }else {
                    printf("rename fail!\n");
                }
    }

int process(int clientSocket, char uname[100]){
	char cmd[BUFF_SIZE];
	char *c1,*c2;
	hello(uname);
	while(1){
		bzero(cur, sizeof(cur));
		recv(clientSocket, cur, BUFF_SIZE, 0);
		printf("Cur:==%s==\n", cur);
		if(strcmp(cur,"FUNCTION_NOT_EXIST") == 0) continue;
		enter(uname,cur);
		fflush(stdin);
		memset(cmd,'\0',(strlen(cmd)+1));
		gets(cmd);
		c1 = get1(cmd);
		c2 = get2(cmd);
		if (!strcmp(c1,"out")) {
			send(clientSocket,c1,BUFF_SIZE,0);
			break;
		}
		if (!strcmp(c1,"help")) { 
			cmdlist(clientSocket);
			continue;
		}
		if (!strcmp(c1,"upfile")) {
			if (upfile(clientSocket,c1,c2))
				continue;
			else {
				conti(clientSocket);
				continue;
			}
		}
		if (!strcmp(c1,"delete")){
			dele(clientSocket,c1,c2);
			continue;
		}
		if (!strcmp(c1,"remove")){
			remo(clientSocket,c1,c2);
			continue;
		}
		if (!strcmp(c1,"downfile")){
		downFile(clientSocket,cmd);
			continue;
		}
		if (!strcmp(c1,"ls")){
			ls(clientSocket,c1,c2);
			continue;
		}
		if(!strcmp(c1,"goto")){
			go(clientSocket,c1,c2);
			continue;
		}
		if(!strcmp(c1,"back")){
			back(clientSocket,c1,c2);
			continue;
		}
		if(!strcmp(c1,"rename")){
        			renameClient(clientSocket,cmd);
        			continue;
        		}
		printf("Command not found!\n");
		conti(clientSocket);
	}
	return 1;
}

