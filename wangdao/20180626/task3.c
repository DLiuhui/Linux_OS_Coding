#include "header.h"

//创建子进程，父进程通过wait回收，获得子进程返回值

int main(int argc, char **argv)
{
	pid_t c_pid = fork();
	if(c_pid < 0){
		perror("fork");
	}else if(c_pid == 0){	//是子进程
		printf("child\n");
		sleep(2);
		printf("child over\n");
		return 3;
	}else{	//退出父进程
		int status;
		printf("parent\n");
		c_pid = wait(&status);	//获得子进程退出状态，为取返回值使用
		if(WIFEXITED(status)){	//如果子进程正常结束
			//WEXITSTATUS，取得子进程返回值
			printf("%d child is over. return %d\n", c_pid, WEXITSTATUS(status));
		}else{
			printf("child running failure\n");
		}
		printf("parent over\n");
		exit(EXIT_SUCCESS);
	}
}
