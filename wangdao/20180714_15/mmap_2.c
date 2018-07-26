#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 100
int main(int argc, char **argv)
{
	char *p_map;
	p_map = (char *)mmap(NULL, BUF_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);	//由于不是用于传输文件，只是父子进程通信，没有文件描述符，使用 匿名 flag
	if(0 == fork())
	{
		sleep(1);
		printf("child:%s\n", p_map);
		sprintf(p_map, "%s", "I am child aaaaa");
		printf("child:%s\n", p_map);
		sprintf(p_map, "%s", "I am child hahahahah");
		munmap(p_map, BUF_SIZE);
		exit(0);
	}
	sprintf(p_map, "%s", "I am father XXX");
	sleep(2);
	printf("parent got :%s\n", p_map);
	munmap(p_map, BUF_SIZE);
	return 0;
}
