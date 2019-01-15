void downfile(int newSocket, char buffer[1024])
{
FILE *f1;
    f1 = fopen("server-test.txt","rb");
                            if(f1 == NULL) {
                                printf("Error! Invalid input file\n");
                                exit(1);
                            }
                            while(1) {
                                fgets(buffer, 1024, f1);
                                if(feof(f1)) {
                                    break;
                                }else {
                                    send(newSocket, buffer, 1024, 0);
                                    bzero(buffer, sizeof(buffer));
                                }
                            }
                            fclose(f1);
                            strcpy(buffer, "endfile123");
                            printf("%s\n", buffer);
                            send(newSocket, buffer, 1024, 0);
                            bzero(buffer, sizeof(buffer));
}

int checkOpCode(int newSocket, char buffer[1024])
{
    char *p;
    char str[1024];
    strcpy(str, buffer);
    p = strtok(str,"|");
    if(strcmp(p, "DOWNFILE")==0){
    	downfile(newSocket, buffer); //downfile server
    	return 1;
    }
    return 0;
}