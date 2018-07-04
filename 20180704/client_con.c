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
	int ret = connect(sfd, (struct sockaddr*)&ser, sizeof(struct sockaddr));
	check_error(-1, ret, "connect");
	char buff[128]={0};
	fd_set sel;
	while(1){
		FD_ZERO(&sel);
		FD_SET(sfd, &sel);
		FD_SET(STDIN_FILENO, &sel);
		ret = select(sfd + 1, &sel, NULL, NULL, NULL);
		if(ret > 0)
		{
			if(FD_ISSET(sfd, &sel)){
				bzero(buff, sizeof(buff));
				if(recv(sfd, buff, sizeof(buff), 0) != 0){
					printf("%s\n", buff);
				}else{
					printf("server: close.\n");	//服务器关闭
					break;
				}
			}
			if(FD_ISSET(STDIN_FILENO, &sel))
			{
				bzero(buff, sizeof(buff));
				read(STDIN_FILENO, buff, sizeof(buff));
				send(sfd, buff, strlen(buff)-1, 0);
			}
		}
	}
	close(sfd);
	return 0;
}
