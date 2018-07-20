#include "header.h"
#include "TCP_server.h"
#include "work_queue.h"
#include "server.h"
#include "r_s_api.h"
//#define QUEUE_CAPACITY 100
//#define TCP_CAPACITY 1000

//const int long_command[] = {4,5};
int exit_fd[2];
void signal_exit(int signal_num)
{
	char exit_flag;
	if(SIGINT == signal_num){
		exit_flag = 'e';
	}
	write(exit_fd[1], &exit_flag, 1);
}


int readline(int fd, char *buffer)
{
	int j = 0;
	char c;
	while(0 < read(fd, &c, 1) && c !='\n' && c != EOF)
	{
		buffer[j++] = c;
	}
	return 1;
}

void give_child_task(int fd, pserver_t server, TCP_protocol_t message)
{
	pnode_t new_task = (pnode_t)calloc(1, sizeof(node_t));
	if(message.order != 4 && message.order != 5)	//非文件I/O指令
	{
		new_task->fd = fd;
		new_task->work_id = message.order;
		strcpy(new_task->option, message.buffer);
		//printf("%s\n", new_task->option);
		pthread_mutex_lock(&server->child_shell_queue->queue_lock);
		queue_insert(server->child_shell_queue, new_task);
		pthread_mutex_unlock(&server->child_shell_queue->queue_lock);
		pthread_cond_signal(&server->child_shell_queue->queue_not_empty);
	}
	else	//是I/O指令，加入IO队列
	{
		new_task->fd = fd;
		new_task->work_id = message.order;
		strcpy(new_task->option, message.buffer);
		//printf("%s\n", new_task->option);
		pthread_mutex_lock(&server->child_IO_queue->queue_lock);
		queue_insert(server->child_IO_queue, new_task);
		pthread_mutex_unlock(&server->child_IO_queue->queue_lock);
		pthread_cond_signal(&server->child_IO_queue->queue_not_empty);
	}
}

int main(int argc, char **argv)
{
	check_argc(argc, 2);
	//读取配置文件
	int fd = open(argv[1], O_RDONLY);
	char ip_conf[15] = {0};
	char port_conf[4] = {0};
	char thread_num_conf[4] = {0};
	readline(fd, ip_conf);
	readline(fd, port_conf);
	readline(fd, thread_num_conf);
	close(fd);
	//初始化
	int ret_num;
	TCP_protocol_t message;
	//注册退出信号
	pipe(exit_fd);
	signal(SIGINT, signal_exit);
	//启动工程
	server_t server;	//主要数据结构
	int thread_num = atoi(thread_num_conf);
	server_init(&server, thread_file_IO, thread_shell, thread_num);
	server_start(&server);
	log_input(server.log_fd, server.log_lock, "net disk server begin to work.");
	//socket
	int socket_fd;
	TCP_server_link(&socket_fd, ip_conf, port_conf);
	//epoll
	int epfd = epoll_create(1);
	struct epoll_event event, ev[TCP_CAPACITY];
	//check_error(-1, epfd, "epoll_ctl");
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &event);
	check_error(-1, ret_num, "epoll_ctl");
	event.data.fd = exit_fd[0];
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fd[0], &event);
	check_error(-1, ret_num, "epoll_ctl");
	//主要工作
	int new_fd;
	int temp_fd;
	int read_fd_get;
	int i, j;
	struct sockaddr_in clientaddr;
	bzero(&clientaddr, sizeof(clientaddr));
	socklen_t addrlen = sizeof(clientaddr);
	while(1)
	{
		read_fd_get = epoll_wait(epfd, ev, TCP_CAPACITY, -1);	//监听
		for(i = 0; i < read_fd_get; i++)
		{
			if(socket_fd == ev[i].data.fd)	//取得连接，加入shell队列
			{
				new_fd = accept(socket_fd, (struct sockaddr*)&clientaddr, &addrlen);
				event.data.fd = new_fd;
				bzero(server.log_buffer, LOG_BUFFER_SIZE);
				sprintf(server.log_buffer, "ip:%s connect", inet_ntoa(clientaddr.sin_addr));
				log_input(server.log_fd, server.log_lock, server.log_buffer);
				ret_num = recv_n(new_fd, (char*)&message, TCP_PROTOCOL_SIZE);	//接收客户机消息
				if(ret_num <= 0)	//对端断开，或出错
				{
					close(new_fd);
					continue;	//继续遍历其他连接句柄
				}
				ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &event);
				check_error(-1, ret_num, "epoll_ctl");
				for(j = 0; j < TCP_CAPACITY; j++)
				{
					if(-1 == server.client_fd[j].fd)
					{
						server.client_fd[j].fd = new_fd;
						server.client_fd[j].work_flag = 1;
						break;
					}
				}
				give_child_task(new_fd, &server, message);
			}
			else if(exit_fd[0] == ev[i].data.fd)	//检测到退出信号
			{
				close(socket_fd);	//关闭socket，不再接收新的网络请求
				char flag;
				ret_num = read(exit_fd[0], &flag, 1);
				if('e' == flag)
				{
					/*插入shell队列*/
					pthread_mutex_lock(&server.child_IO_queue->queue_lock);
					queue_head_insert_exit(server.child_IO_queue);
					//printf("exit IO queue\n");
					pthread_mutex_unlock(&server.child_IO_queue->queue_lock);
					pthread_cond_broadcast(&server.child_IO_queue->queue_not_empty);
					/*插入I/O队列*/
					pthread_mutex_lock(&server.child_shell_queue->queue_lock);
					queue_head_insert_exit(server.child_shell_queue);
					//printf("exit shell queue\n");
					pthread_mutex_unlock(&server.child_shell_queue->queue_lock);
					pthread_cond_broadcast(&server.child_shell_queue->queue_not_empty);
					/*发送信号*/
					for(j = 0; j < server.th_num; j++)
					{
						pthread_join(server.th_id[j], NULL);
						//printf("thread_exit\n");
					}
					printf("exit succ\n");
					database_logout(&server.database);	//退出数据库
					queue_delete(server.child_shell_queue);
					queue_delete(server.child_IO_queue);
					free(server.th_id);
					free(server.client_fd);
					log_input(server.log_fd, server.log_lock, "net disk server close");
					close(server.log_fd);
					return 0;
				}
			}
			else	//是连接中的客户端
			{
				//取得任务
				temp_fd = ev[i].data.fd;
				if(1 == is_client_work(server.client_fd,temp_fd))
				{
					continue;
				}
				ret_num = recv_n(temp_fd, (char*)&message, TCP_PROTOCOL_SIZE);
				if(ret_num <= 0)
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, temp_fd, NULL);
					for(j = 0; j < TCP_CAPACITY; j++)
					{
						if(server.client_fd[j].fd == temp_fd)
						{
							bzero(server.log_buffer, LOG_BUFFER_SIZE);
							sprintf(server.log_buffer, "user:%s disconnect", server.client_fd[j].name);
							log_input(server.log_fd, server.log_lock, server.log_buffer);
							server.client_fd[j].fd = -1;
							server.client_fd[j].work_flag = 0;
							//server.client_fd[j].dir_id = -1;
							bzero(server.client_fd[j].name, 20);
							bzero(server.client_fd[j].path, 512);
							break;
						}
					}
					close(temp_fd);
					continue;
				}
				for(j = 0; j < TCP_CAPACITY; j++)
				{
					if(server.client_fd[j].fd == temp_fd)
					{
						server.client_fd[j].work_flag = 1;
						break;
					}
				}
				give_child_task(temp_fd, &server, message);
			}
		}
	}
	return 0;
}
