#include "header.h"

int main(int argc, char **argv)
{
	check_argc(argc,3);
	int sfd;
	//初始化socket
	sfd = socket(AF_INET,SOCK_STREAM,0);
	check_error(-1, sfd, "socket");
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//点分10进制的IP地址转为网络字节序
	int ret;
	ret = connect(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	check_error(-1, ret, "connect");
	send(sfd, "hello", 6, 0);
	char buf[128]={0};
	recv(sfd, buf, sizeof(buf), 0);
	printf("client receive: %s\n", buf);
	while(1);
	close(sfd);
	return 0;
}
