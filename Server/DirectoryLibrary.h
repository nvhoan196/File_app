#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int CreateFolder(const char *dirPath){
	return mkdir(dirPath,0755);
}