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
		total = total + ret_num;
	}
	return 0;
}
