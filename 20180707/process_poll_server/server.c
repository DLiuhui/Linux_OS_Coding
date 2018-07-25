#include "header.h"

int exit_fds[2];

void signal_exit(int sign_num)
{
	char exit_flag = 'e';
	write(exit_fds[1], &exit_flag, 1);
}

int main(int argc, char **argv)	//传入ip 端口 子进程数
{
	//注册退出信号
	pipe(exit_fds);
	signal(SIGUSR1, signal_exit);
	//开始初始化资源
	check_argc(argc, 4);
	int process_num = atoi(argv[3]);	//子进程数目
	int ret_num;	//函数返回值检验
	int i;	//用于循环计数
	pDataType p_data = (pDataType)calloc(process_num, sizeof(DataType));	//定义全局变量管理进程资源，p_data
	//创建子进程
	make_child_process(p_data, process_num);
	//socket
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	check_error(-1, sfd, "socket");
	int reuse = 1;
	ret_num = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
	check_error(-1, ret_num, "setsockopt");
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port=htons(atoi(argv[2]));//端口号转换为网络字节序
	server.sin_addr.s_addr=inet_addr(argv[1]);//点分10进制的IP地址转为网络字节序
	ret_num = bind(sfd,(struct sockaddr*)&server,sizeof(server));	//bind
	check_error(-1, ret_num ,"bind");
	listen(sfd, process_num);	//listen
	//epoll
	int epfd = epoll_create(1);
	struct epoll_event event, *evs;
	evs = (struct epoll_event*)calloc(process_num+2, sizeof(struct epoll_event));	//监听队列，数目为 子进程数+1（本机），+1（信号触发端口）
	event.data.fd = sfd;
	event.events = EPOLLIN;
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	check_error(-1, ret_num, "epoll_ctl");
	event.data.fd = exit_fds[0];
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fds[0], &event);
	check_error(-1, ret_num, "epoll_ctl");
	//将子进程控制句柄加入监听队列
	for(i = 0; i < process_num; i++)
	{
		event.data.fd = p_data[i].fds;
		ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, p_data[i].fds, &event);
		check_error(-1, ret_num, "epoll_ctl");
	}
	//父进程主要任务
	int read_fd_num;
	int new_fd;
	int j;
	char notbusy_flag;
	while(1)
	{
		read_fd_num=epoll_wait(epfd,evs,process_num+2,-1);
		for(i=0;i<read_fd_num;i++)
		{
			if(evs[i].data.fd==sfd)
			{
				new_fd=accept(sfd,NULL,NULL);
				for(j=0;j<process_num;j++)
				{
					if(FALSE == p_data[j].busy_flag)
					{
						send_fd(p_data[j].fds, new_fd, 0);//发送描述符给子进程
						p_data[j].busy_flag=TRUE;
						printf("pid=%d is busy\n",p_data[j].pid);
						break;
					}
				}
				close(new_fd);
			}
			//到底是哪个子进程通知了父进程自己非忙碌
			for(j=0;j<process_num;j++)
			{
				if(evs[i].data.fd==p_data[j].fds)
				{
					read(p_data[j].fds,&notbusy_flag,1);
					p_data[j].busy_flag = FALSE;
					printf("pid=%d is not busy\n",p_data[j].pid);
				//	sleep(2);
				}
			}
			if(evs[i].data.fd == exit_fds[0])	//侦测退出信号
			{
				close(sfd);	//关闭套接字
				for(j = 0; j < process_num; j++)
				{
					send_fd(p_data[j].fds, 0, -1);
				}
				for(j = 0; j < process_num; j++)
				{
					wait(NULL);
				}
				printf("server exit success\n");
				return 0;
			}
		}
	}
}
