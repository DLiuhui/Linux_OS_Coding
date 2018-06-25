#include "header.h"

int main(int argc, char **argv)
{
	pid_t pid_get = fork();
	if(pid_get < 0)
	{
		perror("fork");
		return -1;
	}else if(pid_get == 0){	//是子进程
			printf("I am child. pid=%d, ppid=%d, euid=%d, egid=%d\n", getpid(), getppid(), geteuid(), getegid());
			while(1);
	}
	else{
			printf("I am parent. pid=%d, ppid=%d, euid=%d, egid=%d\n", getpid(), getppid(), geteuid(), getegid());
			while(1);
	}
	return 0;
}
