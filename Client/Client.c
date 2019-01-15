#include "client.h"

int clientSocket, ret;
struct sockaddr_in serverAddr;
struct sockaddr_in c_addr;

int main(int argc, char *argv[]){

	char buffer[3];
	int c;

	if(argc != 3){
		printf("Please use command : \n");
		printf("./client IPAddress PortNumber\n");
		return 0;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while(c!=3){
		menu();
		fflush(stdin);
		memset(buffer,'\0',(strlen(buffer)+1));
		gets(buffer);
		c=atoi(buffer);
		if(c==1) signup(clientSocket);
			else if(c==2) signin(clientSocket);
				else {
					printf("Good bye!\n");
					break;
				}
	}
	close(clientSocket);
	printf("[-]Disconnected from server.\n");
	exit(1);
	return 0;
}