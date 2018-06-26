#include "header.h"

//创建一个deamon进程，每隔5秒，向/tmp下的文件log里写一句I have sleep 5

int main(int argc, char **argv)
{
	pid_t daemon_pid = fork();
	if(daemon_pid < 0){
		perror("fork");
		exit(EXIT_FAILURE);
	}else if(daemon_pid == 0){	//创建守护进程
		setsid();	//成为新进程与新会话组的组长
		chdir("/");	//工作目录为根目录
		umask(0);
		int i;
		for(i = 0; i < 3; i++){
			close(i);	//关闭文件
		}
		//守护进程工作
		const char *str = "I have sleep 5.\n";
		int fw = open("/tmp/log", O_RDWR, 0666);	//以追加写方式打开，若文件不存在则创建
		while(1){
			write(fw, str, strlen(str));
			sleep(5);
		}
	}else{	//退出父进程
		printf("parent over.\n");
		exit(EXIT_SUCCESS);
	}
}
