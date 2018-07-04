#include "header.h"
//服务器与客户端实时通信
//断开可以重连
int main(int argc, char **argv)
{
	check_argc(argc, 3);
	int sfd;
	//初始化socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	check_error(-1, sfd, "socket");
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[2]));	//端口转化为网络字节序号
	ser.sin_addr.s_addr = inet_addr(argv[1]);	//点分十进制ip地址转化为网络字节序
	int ret = bind(sfd, (struct sockaddr*)&ser, sizeof(ser));
	check_error(-1, ret, "bind");
	listen(sfd, 10);
	int new_fd;
	struct sockaddr_in cli;
	bzero(&cli, sizeof(cli));
	int len = sizeof(cli);
	char buff[128] = {0};
	fd_set rd_fd;
	fd_set tmp_fd;
	FD_ZERO(&tmp_fd);
	FD_SET(STDIN_FILENO, &tmp_fd);
	FD_SET(sfd, &tmp_fd);
	//分时复用
	while(1){
		FD_ZERO(&rd_fd);
		memcpy(&rd_fd, &tmp_fd, sizeof(fd_set));
		ret = select(13, &rd_fd, NULL, NULL, NULL);
		if(ret > 0)
		{
			if(FD_ISSET(sfd, &rd_fd)){	//服务器收到连接请求
				new_fd = accept(sfd, (struct sockaddr*)&cli, &len);
				printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
				FD_SET(new_fd, &tmp_fd);
			}
			if(FD_ISSET(new_fd, &rd_fd)){	//连接情况下收到消息
				bzero(buff, sizeof(buff));
				if(recv(new_fd, buff, sizeof(buff), 0) != 0){
					printf("%s\n", buff);
				}else{
					printf("client: ip = %s, port = %d,  close.\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
					FD_CLR(new_fd, &tmp_fd);
				}
			}
			if(FD_ISSET(STDIN_FILENO, &rd_fd)){	//服务器主动发消息
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				send(new_fd, buff, strlen(buff)-1, 0);
			}
		}
	}
	close(new_fd);	//如果服务器先退出的话，会跳过这两步，会导致socket连接未断开；客户端退出不影响。
	close(sfd);
}
