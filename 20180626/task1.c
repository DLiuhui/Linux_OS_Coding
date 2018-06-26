#include "header.h"

//得到孤儿进程

int main()
{
	pid_t c_pid = fork();
	if(c_pid < 0){
		perror("fork");
	}else if(c_pid == 0){	//是子进程
		printf("child\n");
		while(1);
	}else{	//退出父进程
		printf("parent\n");
		exit(EXIT_SUCCESS);
	}
}
