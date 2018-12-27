#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[]){

	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
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

    FILE *f1;

	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				bzero(buffer, sizeof(buffer));
				recv(newSocket, buffer, BUFF_SIZE, 0);
				if(buffer[0] == '\0'){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					printf("Client %s:%d: %s123\n",inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), buffer);
					if(strcmp(buffer, "downfile")==0){
					    f1 = fopen("server-test.txt","r");
                        if(f1 == NULL) {
                            printf("Error! Invalid input file\n");
                            continue;
                        }
                        while(1) {
                            fgets(buffer, BUFF_SIZE, f1);
                            if(feof(f1)) {
                                break;
                            }else {
                            printf("123%s\n", buffer);
                                send(newSocket, buffer, BUFF_SIZE, 0);
                                bzero(buffer, sizeof(buffer));
                            }
                        }
                        fclose(f1);
                        strcpy(buffer, "endfile123");
                        printf("%s\n", buffer);
                        send(newSocket, buffer, strlen(buffer), 0);
                        bzero(buffer, sizeof(buffer));

					}
				}
			}
		}

	}

	close(newSocket);


	return 0;
}