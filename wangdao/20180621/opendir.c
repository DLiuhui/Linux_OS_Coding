#include "header.h"

int main(int argc, char **argv)
{
	argc_check(argc,2);
	DIR *dir;
	dir = opendir(argv[1]);
	if(!dir){
		perror("opendir error");
		return -1;
	}
	struct dirent *ptr;
	while((ptr = readdir(dir)) != NULL){
		printf("ino=%ld d_reden=%d d_type=%d d_name=%s\n",
				ptr->d_ino, ptr->d_reclen, ptr->d_type, ptr->d_name);
	}
	closedir(dir);
	return 0;
}
