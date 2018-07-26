#include "header.h"
//服务器与客户端实时通信
//断开可以重连
//采用epoll接口
//使用et模式
void change_fd_status(int fd)
{
	int status = fcntl(fd, F_GETFL);
	status = status|O_NONBLOCK;
	fcntl(fd, F_SETFL, status);
}

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
	char buff[10] = {0};
	int epfd = epoll_create(1);
	struct epoll_event event, evs[3];
	event.events = EPOLLIN;	//是否可读
	event.data.fd = STDIN_FILENO;	//监听标准输入句柄
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	check_error(-1, ret, "epoll_ctl");
	event.data.fd = sfd;	//监听服务器句柄
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	check_error(-1, ret, "epoll_ctl");
	int i;
	int retN;
	//分时复用
	while(1){
		ret = epoll_wait(epfd, evs, 3, -1);
		for(i = 0; i < ret; i++)
		{
			if(evs[i].data.fd == sfd)	//sfd可读
			{
				new_fd = accept(sfd, (struct sockaddr*)&cli, &len);
				change_fd_status(new_fd);	//如果不将句柄格式转换为非阻塞，下方读客户端句柄时会无法退出读取状态
				event.data.fd = new_fd;
				event.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &event);
				printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
			}
			if(evs[i].data.fd == new_fd){	//连接情况下收到消息
				while(1)
				{
					bzero(buff, sizeof(buff));
					retN = recv(new_fd, buff, sizeof(buff), 0);
					//sleep(1);
					//printf("retN = %d\n", retN);
					if(retN > 0)
					{
						printf("%s", buff);
					}else if(retN == 0){
						printf("client: ip = %s, port = %d,  close.\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
						event.data.fd = new_fd;
						epoll_ctl(epfd, EPOLL_CTL_DEL, new_fd, &event);
						close(new_fd);
					}else{
						printf("\n");
						break;
					}
				}
			}
			if(EPOLLIN == evs[i].events && evs[i].data.fd == STDIN_FILENO){	//服务器主动发消息，标准输入并未设定成边缘传输模式，所以能够全部读完
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				send(new_fd, buff, strlen(buff)-1, 0);
			}
		}
	}
	close(new_fd);	//如果服务器先退出的话，会跳过这两步，会导致socket连接未断开；客户端退出不影响。
	close(sfd);
}
