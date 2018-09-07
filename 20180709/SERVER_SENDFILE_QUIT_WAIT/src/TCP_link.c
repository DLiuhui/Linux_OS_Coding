#include "TCP_link.h"

//传入套接字句柄，10进制IP地址，端口号，线程数目对应最大监听数
int TCP_link_server(int *socket_fd, char *ip_addr, char *port_num)
{
	int ret_num;
	int reuse_flag = 1;
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	check_TCP(-1, *socket_fd, "socket");
	ret_num = setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR,&reuse_flag, sizeof(int));
	check_TCP(-1, ret_num, "setsockopt");
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port=htons(atoi(port_num));	//端口号转换为网络字节序
	server.sin_addr.s_addr=inet_addr(ip_addr);	//点分10进制的IP地址转为网络字节序
	ret_num = bind(*socket_fd, (struct sockaddr*)&server, sizeof(server));   //bind
	check_TCP(-1, ret_num ,"bind");
#ifdef TCP_CAPACITY
	ret_num = listen(*socket_fd, TCP_CAPACITY);
#else
	ret_num = listen(*socket_fd, 10);	//如果未定义网络连接数上限则用默认值10
#endif
	check_TCP(-1, ret_num, "listen");
	return 1;
}
