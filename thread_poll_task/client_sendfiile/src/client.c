#include "header.h"

int fd;
int sfd;
//服务器
void signal_exit(int sig_num)
{
	if(SIGINT == sig_num)
	{
		close(fd);
		close(sfd);
		//printf("client close succ\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	check_argc(argc, 3);
	signal(SIGINT, signal_exit);
	int ret_num;
	//socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	check_error(-1, sfd, "socket");
	struct sockaddr_in client;
	bzero(&client, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(atoi(argv[2]));
	client.sin_addr.s_addr = inet_addr(argv[1]);
	ret_num = connect(sfd, (struct sockaddr*)&client, sizeof(struct sockaddr));	//connect
	check_error(-1, ret_num, "connect");
	//收文件
	int len;
	char buf[1000] = {0};
	//收文件名字
	recv_n(sfd, (char*)&len, 4);
	recv_n(sfd, buf, len);
	printf("buf=%s\n", buf);
	fd = open(buf, O_RDWR|O_CREAT, 0666);
	check_error(-1, fd, "open");
	//接收文件大小
	off_t f_size = 0;
	recv_n(sfd, (char*)&len, 4);
	recv_n(sfd, (char*)&f_size, len);
	while(1)
	{
		//接收文件
		ret_num = recv_file(sfd, fd, buf, sizeof(buf), f_size);
		if(ret_num <= 0)
		{
			printf("recv_file error\n");
			break;
		}
		else
		{
			printf("recv_file succ\n");
			break;
		}
	}
	close(fd);
	close(sfd);
	return 0;
}
