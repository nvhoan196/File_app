void downFile(int clientSocket, char buffer[1024])
{
    FILE *f1;

    char *p;
    char str[1024];
    char *fileName = (char *)malloc(255);

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
        			if(strcmp(buffer, "endfile123") == 0){
        			printf("****File download success!****\n");
        			break;
        			}
                    fprintf(f1,"%s",buffer);
        		}
        }
		fclose(f1);
}