#include "header.h"

int main()
{
	printf("haha\n");
	pid_t pid = fork();
	if(!pid)
	{
		printf("child.\n");
		return 1;
	}
	else
	{
		printf("parent.\n");
	}
	printf("xxxx%d\n", (pid == 0) ? 1 : 0);
	return 1;
}
