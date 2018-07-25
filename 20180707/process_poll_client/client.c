#include "header.h"

//服务器

int main(int argc, char **argv)
{
	check_argc(argc, 3);
	int ret_num;
	//socket
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
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
	int fd = open(buf, O_RDWR|O_CREAT, 0666);
	check_error(-1, fd, "open");
	//接收文件大小
	off_t f_size = 0;
	double download_file = 0;
	recv_n(sfd, (char*)&len, 4);
	recv_n(sfd, (char*)&f_size, len);
	//根据时间打印下载进度
	time_t start = time(NULL);
	time_t end = start;
	while(1)
	{
		ret_num = recv_n(sfd, (char*)&len, 4);
		if(ret_num != -2 && len > 0)
		{
			ret_num = recv_n(sfd, buf, len);
			if(-2 == ret_num)
			{
				printf("download percent %5.2f%c\n", download_file/f_size*100, '%');
				break;
			}
			write(fd, buf, len);
			download_file += len;
			end = time(NULL);
			if(end - start > 1)
			{
				printf("download percent %5.2f%c\r", download_file/f_size*100, '%');
				fflush(stdout);
				start = end;
			}
		}
		else
		{
			 printf("download percent %5.2f%c\n", download_file/f_size*100, '%');
			 break;
		}
	}
	close(fd);
	close(sfd);
}
