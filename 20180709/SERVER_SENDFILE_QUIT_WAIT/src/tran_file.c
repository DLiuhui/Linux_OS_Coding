#include "tran_file.h"
int tran_file(int new_fd, char *filename)
{
	Train t;
	int ret_num;
	//先发送文件名
	t.len = strlen(filename);
	strcpy(t.buf, filename);
	ret_num = send_n(new_fd, (char*)&t, 4+t.len);
	if(-1 == ret_num)
	{
		close(new_fd);
		return -1;
	}
	printf("file name send\n");
	//发送文件大小
	struct stat f_stat;
	stat(filename, &f_stat);
	t.len = sizeof(f_stat.st_size);
	memcpy(t.buf, &f_stat.st_size, sizeof(off_t));
	ret_num = send_n(new_fd, (char*)&t, 4+t.len);
	if(-1 == ret_num)
	{
		close(new_fd);
		return -1;
	}
	printf("file size send\n");
	//发送文件内容
	int fd = open(filename, O_RDONLY);
	check_error(-1, fd, "open");
	printf("open file ok\n");
	if(sendfile(new_fd, fd, 0, f_stat.st_size)<0)
	{
		printf("%d\n", errno);
		printf("sendfile error\n");
		close(new_fd);
		return -1;
	}
	/*
	off_t total_send = 0;
	off_t send_count = 0;
	off_t offset = 0;
	off_t current_size = f_stat.st_size;
	while((send_count = sendfile(new_fd, fd, &offset, current_size)) != -1)
	{
		if((total_send += send_count) == f_stat.st_size)
		{
			break;
		}
		else
		{
			current_size = current_size - send_count;
			offset = offset + send_count;
		}
	}
	*/
	printf("sendfile ok\n");
	close(new_fd);
	return 0;
}

//收发过程中存在收发双方处理速度不匹配的问题
//需要在发送/接收时校验收发的数据量（检验）
//并进行处理

int send_n(int sfd, char *buf, int buflen)
{
	int total = 0;
	int ret_num;
	while(total < buflen)
	{
		ret_num = send(sfd, buf+total, buflen-total, 0);    //需要对buf设置偏移量，长度要减去已经发送的部分
		check_error(-1, ret_num, "send");
		if(-1 == ret_num)   //发送失败
		{
			printf("client is close\n");
			return -1;
		}
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
		ret_num = recv(sfd, buf+total, buflen-total, 0);    //需要对buf设置偏移量，长度要减去已经发送的部分
		check_error(-1, ret_num, "recv");
		total = total + ret_num;
	}
	return 0;
}

