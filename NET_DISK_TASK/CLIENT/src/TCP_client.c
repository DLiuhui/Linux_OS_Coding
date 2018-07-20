#include "TCP_client.h"

int TCP_client_link(int *sfd, char *ip, char *port)
{
	int ret_num;
	//char buffer[100];	//可以用于接收服务器信息
	*sfd = socket(AF_INET, SOCK_STREAM, 0);
	check_error(-1, *sfd, "socket");
	struct sockaddr_in client;
	bzero(&client, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(atoi(port));
	client.sin_addr.s_addr = inet_addr(ip);
	ret_num = connect(*sfd, (struct sockaddr*)&client, sizeof(struct sockaddr)); //connect
	check_error(-1, ret_num, "connect");
	//接收服务器的连接消息
	//recv(*sfd, buffer, 100, 0);
	printf("Connect success\n");
	return 1;
}
