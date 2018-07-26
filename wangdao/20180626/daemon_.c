#include "header.h"

int main(int argc, char **argv)
{
	pid_t daemon_pid = fork();
	if(daemon_pid < 0){
		perror("fork");
		exit(EXIT_FAILURE);
	}else if(daemon_pid == 0){
		setsid();	//成为会话组组长
		chdir("/");	//设置工作目录为根目录
		umask(0);	//重设文件访问权限码
		int fd;
		for(fd = 0; fd < 3; fd++)	//关闭继承的文件句柄，此处未开启新文件，只需摇关闭0 1 2
			close(fd);
		//守护进程执行部分
		char buffer[25] = {0};
		struct tm *now_time;
		time_t get_time;
		while(1){	//守护进程
			time(&get_time);	//更新现在时间
			now_time = gmtime(&get_time);
			sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
					(1900 + now_time->tm_year), (1 + now_time->tm_mon), (now_time->tm_mday),
					(8 + now_time->tm_hour), (now_time->tm_min), (now_time->tm_sec));
			syslog(LOG_INFO, "Time get: %s\n", buffer);
			sleep(60);	//挂起一段时间再次记录
		}
	}else{
		exit(EXIT_SUCCESS);
	}
}
