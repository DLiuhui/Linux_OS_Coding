#include "header.h"

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
	char buff[128] = {0};
	recv(new_fd, buff, sizeof(buff), 0);
	printf("server receive: %s\n", buff);
	send(new_fd, "world", 5, 0);
	close(new_fd);
	close(sfd);
}
