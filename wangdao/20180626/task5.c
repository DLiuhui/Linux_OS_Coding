#include "header.h"

//5、创建一个无名管道，fork子进程，然后子进程向管道写端写Hello，父进程接收数据并打印

int main(int argc, char **argv)
{
	int fds[2] = {0};
	pipe(fds);
	char buffer[100] = {'\0'};
	pid_t n_pid = fork();
	if(n_pid < 0){
		perror("fork");
		return -1;
	}else if(n_pid == 0){
		close(fds[0]);
		sprintf(buffer, "hello");
		sleep(2);
		write(fds[1], buffer, strlen(buffer));
		return 1;
	}
	else{
		close(fds[1]);
		while(read(fds[0], buffer, sizeof(buffer)) < 0)	//等待，直到子进程写入管道
			;
		puts(buffer);
		return 0;
	}
}
