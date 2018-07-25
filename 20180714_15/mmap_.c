#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		perror("argc");
		return -1;
	}
	int fd = -1;
	struct stat sb;
	char *mmaped = NULL;

	fd = open(argv[1], O_RDWR);
	if(fd < 0)
	{
		printf("open error\n");
		return -1;
	}
	if(stat(argv[1], &sb) < 0)
	{
		printf("stat error\n");
		goto err;
	}

	mmaped = (char*)mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(mmaped == (char*)-1)
	{
		printf("mmap error\n");
		goto err;
	}

	/*内存映射完毕，即使关闭文件也能够操作内存*/
	close(fd);
	//printf("%s\n", mmaped);
	mmaped[10] = 'a';
	/*
	if(msync(mmaped, sb.st_size, MS_SYNC) < 0)
	{
		printf("msync error\n");
		goto err;
	}
	*/
	munmap(mmaped, sb.st_size);
	return 0;

err:
	if(fd > 0)
	{
		close(fd);
	}
	if(mmaped != (char*)-1)
		munmap(mmaped, sb.st_size);
	return -1;
}
