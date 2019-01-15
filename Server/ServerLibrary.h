#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FileLibrary.h"
#include "DirectoryLibrary.h"

// return 0 if success
// return -1 if error
int MakeFolder(const char *curPath, const char *folderName){
	char dirPath[1024];

	bzero(dirPath, sizeof(dirPath));

	strcat(dirPath,curPath);
	strcat(dirPath,"/");
	strcat(dirPath,folderName);

	return CreateFolder(dirPath);
}

// return 0 if success
// return -1 if error
int UpdateFile(const char *curPath,const char *fileName,int sockfd){
	char filePath[1024];


	bzero(filePath, sizeof(filePath));

	strcat(filePath,curPath);
	strcat(filePath,"/");
	strcat(filePath,fileName);

	if(CheckFileExist(filePath)==0){
		RecvFile(sockfd,filePath);
		return 0;
	}
	return -1;
}

// return 0 if success
// return -1 if error
int DeleteFile(const char *curPath, const char *fileName){
	char filePath[1024];


	bzero(filePath, sizeof(filePath));

	strcat(filePath,curPath);
	strcat(filePath,"/");
	strcat(filePath,fileName);

	if(CheckFileExist(filePath)==0){
		return RemoveFile(filePath);
	}
	return -1;
}

// return 0 if success
// return -1 if error
int MoveFile(const char *curPath,const char *fileName, const char *newPath){
	char filePath[1024];
	char newFilePath[1024];

	bzero(filePath, sizeof(filePath));
	bzero(newFilePath, sizeof(filePath));

	strcat(filePath,curPath);
	strcat(filePath,"/");
	strcat(filePath,fileName);

	strcat(newFilePath,newPath);
	strcat(newFilePath,"/");
	strcat(newFilePath,fileName);

	return Move_File(filePath,newFilePath);
}