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







///// 
int rmtree( char path[])
{
    size_t path_len;
    char *full_path;
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;

    // stat for the path
    stat(path, &stat_path);

    // if path does not exists or is not dir - exit with status -1
    if (S_ISDIR(stat_path.st_mode) == 0) {
        fprintf(stderr, "%s: %s\n", "Is not directory", path);
        return 0;
    }

    // if not possible to read the directory for this user
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
        return 0;
    }

    // the length of the path
    path_len = strlen(path);

    // iteration through entries in the directory
    while ((entry = readdir(dir)) != NULL) {

        // skip entries "." and ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        // determinate a full path of an entry
        full_path = calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, entry->d_name);

        // stat for the entry
        stat(full_path, &stat_entry);

        // recursively remove a nested directory
        if (S_ISDIR(stat_entry.st_mode) != 0) {
            rmtree(full_path);
            continue;
        }

        // remove a file object
        if (unlink(full_path) == 0)
            printf("Removed a file: %s\n", full_path);
        else
            printf("Can`t remove a file: %s\n", full_path);
    }

    // remove the devastated directory and close the object of it
    if (rmdir(path) == 0)
        printf("Removed a directory: %s\n", path);
    else
        printf("Can`t remove a directory: %s\n", path);
    
    return 1;
    closedir(dir);
}
