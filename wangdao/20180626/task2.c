#include "header.h"

//创建僵尸进程

int main(int argc, char **argv)
{
	pid_t c_pid = fork();
	if(c_pid < 0){
		perror("fork");
	}else if(c_pid == 0){	//是子进程
		printf("child\n");
		exit(EXIT_SUCCESS);
	}else{	//退出父进程
		printf("parent\n");
		while(1);
	}
}
