#include "header.h"

//收发过程中存在收发双方处理速度不匹配的问题
//需要在发送/接收时校验收发的数据量（检验）
//并进行处理

int send_n(int sfd, char *buf, int buflen)
{
	int total = 0;
	int ret_num;
	while(total < buflen)
	{
		ret_num = send(sfd, buf+total, buflen-total, 0);	//需要对buf设置偏移量，长度要减去已经发送的部分
		total = total + ret_num;
	}
	return 0;
}


int recv_n(int sfd, char *buf, int buflen)
{
	int total = 0;
	int ret_num;
	while(total < buflen)
	{
		ret_num = recv(sfd, buf+total, buflen-total, 0);	//需要对buf设置偏移量，长度要减去已经发送的部分
		if(0 == ret_num)
		{
			return -2;
		}
		total = total + ret_num;
	}
	return 0;
}

int recv_file(int sfd, int fd, char *buf, int buflen, off_t size_file)
{
	double total_download = 0;
	int ret_num = 0;
	time_t start, end;
	start = time(NULL);
	while(total_download < size_file)
	{
		ret_num = recv(sfd, buf, buflen, 0);
		if(ret_num == -1)
		{
			printf("recv_file error in func\n");
			return -1;
		}
		else
		{
			if(ret_num == 0)
			{
				printf("server close\n");
				return -1;
			}
			total_download += ret_num;
			end = time(NULL);
			if(end - start > 1)
			{
				printf("download percent %5.2f%c\r", total_download/size_file*100, '%');
				fflush(stdout);
				start = end;
			}
			write(fd, buf, ret_num);
		}
	}
	printf("download percent %5.2f%c\n", total_download/size_file*100, '%');
	return 1;
}
