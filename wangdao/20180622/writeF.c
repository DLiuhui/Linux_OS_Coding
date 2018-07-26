#include "header.h"

#define PATH 512
#define ROOT 0664
int main(int argc, char **argv)
{
	argc_check(argc, 2);
	int fp;
	fp = open(argv[1], O_RDWR|O_CREAT, ROOT);
	if(-1 == fp)
	{
		perror("open");
		return -1;
	}
	char c[] = "hahahah";
	int ret = write(fp, c, sizeof(c));
	if(ret == -1)
	{
		perror("write");
		return -1;
	}
	printf("fp=%d, ret=%d\n", fp, ret);
	return 0;
}
