#include "header.h"
//UDP客户端

int main(int argc, char **argv)
{
	check_argc(argc,3);
	int sfd;
	//初始化socket
	sfd = socket(AF_INET,SOCK_DGRAM,0);
	check_error(-1, sfd, "socket");
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//点分10进制的IP地址转为网络字节序
	char buff[128]={0};
	int epfd = epoll_create(1);
	struct epoll_event event, evs[2];	//监听sfd和stdin
	event.events = EPOLLIN;
	event.data.fd = STDIN_FILENO;
	int ret;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	check_error(-1, ret, "epoll_ctl");
	event.data.fd = sfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	check_error(-1, ret, "epoll_ctl");
	int retN, i;
	struct sockaddr_in cli;
	while(1){
		ret = epoll_wait(epfd, evs, 2, -1);
		for(i = 0; i < ret; i++)
		{
			if(evs[i].data.fd == sfd)
			{
				bzero(buff, sizeof(buff));
				retN = recvfrom(sfd, buff, sizeof(buff), 0, NULL, NULL);
				check_error(-1, retN, "recvfrom");
				printf("%s\n", buff);
			}
			if(evs[i].data.fd == STDIN_FILENO)
			{
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				retN = sendto(sfd, buff, strlen(buff)-1, 0, (struct sockaddr*)&ser, sizeof(ser));	//只往服务器发送数据
				check_error(-1, retN, "sendto");
			}
		}
	}
	close(sfd);
	return 0;
}
