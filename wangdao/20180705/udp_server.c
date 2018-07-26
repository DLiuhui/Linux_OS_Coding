#include "header.h"
//服务器与客户端实时通信
//断开可以重连
//采用epoll接口
//UDP
int main(int argc, char **argv)
{
	check_argc(argc, 3);
	int sfd;
	//初始化socket
	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	check_error(-1, sfd, "socket");
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[2]));	//端口转化为网络字节序号
	ser.sin_addr.s_addr = inet_addr(argv[1]);	//点分十进制ip地址转化为网络字节序
	int ret = bind(sfd, (struct sockaddr*)&ser, sizeof(ser));
	check_error(-1, ret, "bind");
	struct sockaddr_in cli;
	bzero(&cli, sizeof(cli));
	int len = sizeof(cli);
	char buff[128] = {0};
	int epfd = epoll_create(1);
	struct epoll_event event, evs[2];
	event.events = EPOLLIN;	//是否可读
	event.data.fd = STDIN_FILENO;	//监听标准输入句柄
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	check_error(-1, ret, "epoll_ctl");
	event.data.fd = sfd;	//监听服务器句柄，UDP没有accept过程，所以只需要监听sfd本服务器句柄
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	check_error(-1, ret, "epoll_ctl");
	int i;
	int retN;
	//分时复用
	while(1){
		ret = epoll_wait(epfd, evs, 2, -1);
		for(i = 0; i < ret; i++)
		{
			if(evs[i].data.fd == sfd)	//sfd可读
			{
				bzero(buff, sizeof(buff));
				retN = recvfrom(sfd, buff, sizeof(buff)-1, 0, (struct sockaddr*)&cli, &len);
				check_error(-1, retN, "recvfrom");
				printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
				printf("%s\n", buff);
			}
			if(evs[i].data.fd == STDIN_FILENO){	//服务器主动发消息
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				retN = sendto(sfd, buff, strlen(buff)-1, 0, (struct sockaddr*)&cli, sizeof(cli));
				check_error(-1, retN, "sendto");
			}
		}
	}
	close(sfd);
	return 0;
}
