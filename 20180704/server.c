#include "header.h"
//服务器与客户端实时通信

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
	new_fd = accept(sfd, (struct sockaddr*)&cli, &len);
	printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
	char buff[128] = {0};
	fd_set sel;
	//分时复用
	while(1){
		FD_ZERO(&sel);
		FD_SET(new_fd, &sel);
		FD_SET(STDIN_FILENO, &sel);
		ret = select(new_fd + 1, &sel, NULL, NULL, NULL);
		if(ret > 0)
		{
			if(FD_ISSET(new_fd, &sel)){
				bzero(buff, sizeof(buff));
				if(recv(new_fd, buff, sizeof(buff), 0) != 0){
					printf("%s\n", buff);
				}else{
					printf("client:%d close.\n", new_fd);
					break;
				}
			}
			if(FD_ISSET(STDIN_FILENO, &sel))
			{
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				send(new_fd, buff, strlen(buff)-1, 0);
			}
		}
	}
	close(new_fd);	//如果服务器先退出的话，会跳过这两步，会导致socket连接未断开；客户端退出不影响。
	close(sfd);
}
