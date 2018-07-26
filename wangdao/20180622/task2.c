#include "header.h"
#include <sys/mman.h>
#define ROOT 0644

int main(int argc, char **argv)
{
	argc_check(argc,2);
	int fd;
	fd=open(argv[1],O_RDWR);
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	char *p = (char*)mmap(NULL, 5, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(p == (char*)-1)
	{
		perror("mmap");
		return -1;
	}
	printf("%s\n", p);
	//此处必须保证p的地址不变，不然munmap出错。
	//p所指地址要对应mmap的返回地址
	strcpy(p,"world");
	int ret = munmap(p, 5);
	if(-1 == ret)
	{
		perror("munmap");
		return -1;
	}
	return 0;
}
