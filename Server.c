#include "hoannv.h"

int main(int argc, char *argv[]){
	Alist *root;
	root=(Alist*)malloc(sizeof(Alist));
	readfile(root);

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024],op[3],tmp[255];
	int len,i;
	pid_t childpid;

	if(argc != 2){
		printf("Please use command : \n");
		printf("./server PortNumber\n");
		return 0;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %s\n", argv[1]);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){		//process
				bzero(buffer, sizeof(buffer));
				memset(buffer,'\0',(strlen(buffer)+1));
				len=recv(newSocket, buffer, BUFF_SIZE, 0);
				if(buffer[0] == '\0'){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{					
					memset(op,'\0',(strlen(op)+1));
					op[0]=buffer[0]; op[1]=buffer[1];
					for (i = 3; i < len; i++)	tmp[i-3]=buffer[i];

					if (strcmp("SU", op)==0) 
						if (signup(newSocket,root,tmp)) continue;

					if (strcmp("SI", op)==0) 
						if (signin(newSocket,root,tmp)) continue;






					bzero(buffer, sizeof(buffer));
				}
			}
		}

	}

	close(newSocket);
	return 0;
}