#include "header.h"

int main(int argc, char **argv)
{
	pid_t pid_get;
	pid_get = fork();
	if(pid_get < 0)
	{
		perror("fork");
		return -1;
	}else if(pid_get == 0){
		sleep(1);
		printf("I am child. pid=%d, ppid=%d\n", getpid(), getppid());
		printf("child: euid=%d, egid=%d\n", geteuid(), getegid());
	}else{
		printf("I am parent. pid=%d, ppid=%d\n", getpid(), getppid());
		sleep(2);
		printf("parent pid_get=%d\n", pid_get);
		printf("child: euid=%d, egid=%d\n", geteuid(), getegid());
	}
	return 0;
}
