#include "header.h"
#include "r_s_api.h"
#include "TCP_client.h"
#include "client.h"

const char *command_list[] = {"cd", "ls", "remove", "pwd", "gets", "puts"};
const int command_list_size = sizeof(command_list)/sizeof(char*);


void signal_exit(int sig_num)
{
	if(SIGINT == sig_num)
	{
		printf("client close\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	check_argc(argc, 3);	//输入ip, port
	signal(SIGINT, signal_exit);
	int ret_num;
	client_command_t user_input_command;
	user_message_t user;
	//socket
	int socket_fd;
	ret_num = TCP_client_link(&socket_fd, argv[1], argv[2]);
	check_error(-1, ret_num, "TCP_client_link");
	//用户登录
	if(0 >= user_login(socket_fd, &user))
	{
		close(socket_fd);
		return -1;
	}
	//epoll
	int epfd = epoll_create(1);
	struct epoll_event event, ev;
	/*
	check_error(-1, epfd, "epoll_ctl");
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &event);
	check_error(-1, ret_num, "epoll_ctl");
	*/
	event.events = EPOLLIN;
	event.data.fd = STDIN_FILENO;
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	check_error(-1, ret_num, "epoll_ctl");
	char buffer[1000];
	int epoll_fd_get;	//接收epoll监听到的描述符数目
	int i;
	while(1)
	{
		epoll_fd_get = epoll_wait(epfd, &ev, 1, -1);  //监听
		for(i = 0; i < epoll_fd_get; i++)
		{
			if(socket_fd == ev.data.fd)	//如果是服务发送信息
			{
				;
				//do_server_task(socket_fd, &user_input_command);	//执行服务器操作
			}
			if(STDIN_FILENO == ev.data.fd)	//如果是用户输入
			{
				bzero(buffer, sizeof(buffer));
				ret_num = read(STDIN_FILENO, buffer, 1000);
				check_error(-1, ret_num, "read");
				if(buffer[0] == '\n')	//输入的是回车，什么都不做
				{
					;
				}
				else if(ret_num < 2 || -1 == get_user_order(&user_input_command, buffer, command_list, command_list_size))
				{
					printf("Invalid command, please input again\n");
					bzero(buffer, 1000);
				}
				else
				{
					//printf("do user task\n");
					//printf("%d:%s\n", user_input_command.id, user_input_command.option);
					ret_num = do_user_task(socket_fd, &user_input_command);	//执行用户操作
					if(-1 == ret_num)
					{
						continue;
					}
					else if(0 == ret_num)
					{
						printf("server close\n");
						break;
					}
				}
			}
		}
	}
	close(socket_fd);
	return 0;
}
