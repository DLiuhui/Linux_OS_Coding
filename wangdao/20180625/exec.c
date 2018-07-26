#include "header.h"

int main(int argc, char **argv)
{
	argc_check(argc, 2);
	int count = 0;	//计数器
	int ret = 0;
	char buff[50];
	while(1)
	{
		ret = open(argv[1], O_RDWR);
		if(ret == -1)
		{
			perror("open");
			return -1;
		}
		buff[count] = (char)count++;
		if(write(ret, buff, count) == -1)
		{
			perror("write");
			return -1;
		}
		printf("%d\n", count);
		if(count == 20)
		{
			if(execl("./add", "add", "3", "4", NULL) == -1)
			{
				perror("excel");
				return -1;
			}
			return 0;
		}
	}
	return 0;

}
