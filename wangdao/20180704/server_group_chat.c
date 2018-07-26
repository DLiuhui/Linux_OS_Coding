#include "header.h"
//服务器与客户端实时通信
//断开可以重连
//可以多客户端连接服务器，聊天室
//使用数组
#define N 10
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
	struct sockaddr_in cli;
	bzero(&cli, sizeof(cli));
	int len = sizeof(cli);
	char buff[128] = {0};
	int new_fd[N];
	memset(new_fd, -1, sizeof(new_fd));
	int i, j, get_fd;	//遍历客户机数组
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
				get_fd = accept(sfd, (struct sockaddr*)&cli, &len);
				for(i = 0; i < N; i++){
					if(new_fd[i] == -1){
						new_fd[i] = get_fd;
						break;
					}
				}
				printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
				FD_SET(new_fd[i], &tmp_fd);
			}
			for(i = 0; i < N; i++){
				if(new_fd[i] != -1 && FD_ISSET(new_fd[i], &rd_fd)){	//连接情况下收到消息，服务器收到对应消息后应该将消息发给所有连接者
					bzero(buff, sizeof(buff));
					if(recv(new_fd[i], buff, sizeof(buff), 0) != 0){
						printf("%s\n", buff);	//服务器内部打印
						for(j = 0; j < N; j++)
							new_fd[j] != -1 && j != i && send(new_fd[j], buff, strlen(buff), 0);

					}else{
						close(new_fd[i]);	//对应客户断开，服务器应该释放断开客户占用的资源
						printf("client: %d close.\n", new_fd[i]);
						FD_CLR(new_fd[i], &tmp_fd);
						new_fd[i] = -1;
					}
				}
			}
			if(FD_ISSET(STDIN_FILENO, &rd_fd)){	//服务器主动发消息，向所有连接的客户发送消息
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				for(i = 0; i < N; i++)
					new_fd[i] != -1 && send(new_fd[i], buff, strlen(buff)-1, 0);
			}
		}
	}
	close(sfd);	//如果服务器先退出的话，会跳过这两步，会导致socket连接未断开；客户端退出不影响。
}
