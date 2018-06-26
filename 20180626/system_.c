#include "header.h"

int main(int argc, char **argv)
{
	if(!fork())
	{
		printf("I am child.\n");
		int ret = execl("/bin/ls", "ls", NULL);
		printf("%d\n", ret);
		printf("haha\n");
		while(1);
	}else{
		printf("I am parent.\n");
		while(1);
	}
	return 0;
}
