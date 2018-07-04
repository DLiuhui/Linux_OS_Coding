#include "header.h"
#include "LinkList.h"
//服务器与客户端实时通信
//断开可以重连
//可以多客户端连接服务器，聊天室
//使用链表
int equal(dataType key, dataType data)
{
	return (key.client_fd == data.client_fd) ? 1 : 0;
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
	struct sockaddr_in cli;
	bzero(&cli, sizeof(cli));
	int len = sizeof(cli);
	char buff[128] = {0};
	char writeBuff[150] = {0};
	//链表存储
	List head, rear;
	InitList(&head, &rear);
	int get_fd;	//接收临时的客户机描述符
	dataType new_c;	//接收临时客户信息
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
				if(get_fd != -1){
					new_c.client_fd = get_fd;
					strcpy(new_c.name, "\0");
					send(get_fd, "Input your name:", 16, 0);
					listHeadInsert(&head, &rear, new_c);
					printf("client connect:ip = %s, port = %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
					FD_SET(get_fd, &tmp_fd);
				}
			}
			node *c_node = head;
			while(c_node->next){
				c_node = c_node->next;
				if(FD_ISSET(c_node->data.client_fd, &rd_fd)){	//连接情况下收到消息，服务器收到对应消息后应该将消息发给所有连接者
					bzero(buff, sizeof(buff));
					if(recv(c_node->data.client_fd, buff, sizeof(buff), 0) != 0){
						if(c_node->data.name[0] == '\0')	//还未赋名字
							strcpy(c_node->data.name, buff);
						else{
							bzero(writeBuff, sizeof(writeBuff));
							sprintf(writeBuff, "%s:%s", c_node->data.name, buff);
							printf("%s\n", writeBuff);	//服务器内部打印
							//全网打印（除了发送者）
							node *w_node = head;
							while(w_node->next){
								w_node = w_node->next;
								w_node->data.client_fd != c_node->data.client_fd && send(w_node->data.client_fd, writeBuff, strlen(writeBuff), 0);
							}
						}
					}else{
						close(c_node->data.client_fd);	//对应客户断开，服务器应该释放断开客户占用的资源
						printf("client: %d close.\n", c_node->data.client_fd);
						FD_CLR(c_node->data.client_fd, &tmp_fd);
						listDeleteByKey(&head, &rear, c_node->data, &equal);
					}
				}
			}
			if(FD_ISSET(STDIN_FILENO, &rd_fd)){	//服务器主动发消息，向所有连接的客户发送消息
				bzero(buff, sizeof(buff));
				bzero(writeBuff, sizeof(writeBuff));
				read(STDIN_FILENO, buff, sizeof(buff));
				sprintf(writeBuff, "SERVER:%s", buff);
				node *c_node = head;
				while(c_node->next)
				{
					c_node = c_node->next;
					send(c_node->data.client_fd, writeBuff, strlen(writeBuff)-1, 0);
				}
			}
		}
	}
	deleteList(&head, &rear);
	close(sfd);	//如果服务器先退出的话，会跳过这两步，会导致socket连接未断开；客户端退出不影响。
}
