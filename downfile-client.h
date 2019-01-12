void downFile(int clientSocket, char buffer[1024])
{
    FILE *f1;
    f1 = fopen("result.txt","w");
        if(f1 == NULL) {
                                    printf("Error! Invalid input file\n");
                                    exit(1);
                                }
        while(1) {
        if(recv(clientSocket, buffer, 1024, 0) < 0){
        			printf("[-]Error in receiving data.\n");
        			break;
        		}else{
        			if(strcmp(buffer, "endfile123") == 0){
        			printf("****File download success!****\n");
        			break;
        			}
                    fprintf(f1,"%s",buffer);
        		}
        }
		fclose(f1);
}