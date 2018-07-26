#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#define MAXLINE 10
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 2000
#define INFTIM 1000

//线程池任务队列结构体
struct task
{
	int fd; //需要读写的文件描述符
	struct task *next; //下一个任务
};

//用于读写两个的两个方面传递参数
struct user_data
{
	int fd;
	unsigned int n_size;
	char line[MAXLINE];
};

//声明 epoll_event 结构体的变量,ev 用于注册事件,数组用于回传要处理的事件
struct epoll_event ev, events[20];
int epfd;
pthread_mutex_t mutex;
pthread_cond_t cond1;
struct task *readhead = NULL, *readtail = NULL, 
			*writehead = NULL;

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0)
	{
		perror("fcntl(sock,GETFL)");
		exit(1);
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0)
	{
		perror("fcntl(sock,SETFL,opts)");
		exit(1);
	}
}

void *readtask(void *args)
{
	int fd = -1;
	unsigned int n;
	struct user_data *data = NULL;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		while(readhead == NULL)	//没有读任务
			pthread_cond_wait(&cond1, &mutex);
		fd = readhead->fd;
		//从任务队列取出一个读任务
		struct task *tmp = readhead;
		readhead = readhead->next;
		free(tmp);
		pthread_mutex_unlock(&mutex);
		data = (struct user_data*)malloc(struct user_data);
		data->fd = fd;
		if((n = read(fd, data->line, MAXLINE)) < 0)
		{
			if(errno = ECONNRESET)
			{
				close(fd);
			}else
				printf("readline error\n");
			if(data != NULL)
				free(data);
		}else if(n == 0)
		{
			close(fd);
			printf("Client close connect!\n");
			if(data != NULL)
				free(data);
		}else{
			data->n_size = n;
			ev.data.ptr = data;
			ev.events = EPOLLOUT|EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
		}
	}
}

int main()
{
	int i, maxi, listenfd, connfd, sockfd, nfds;
	pthread_t tid1, tid2;
	struct task *new_task = NULL;
	struct user_data *rdata = NULL;
	socklen_t clilen;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond1, NULL);
	//初始化用于读线程池的线程
	pthread_create(&tid1, NULL, readtask, NULL);
	pthread_create(&tid2, NULL, readtask, NULL);
	//socket
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int))
		//把 socket 设置为非阻塞方式
		setnonblocking(listenfd);
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	char *local_addr = "192.168.92.128";
	serveraddr.sin_addr = inet_addr(local_addr);//htons(SERV_PORT);
	serveraddr.sin_port = htons(SERV_PORT);
	bind(listenfd, (sockaddr *) &serveraddr, sizeof(serveraddr));
	listen(listenfd, LISTENQ);
	//epoll
	//设置与要处理的事件相关的文件描述符
	ev.data.fd = listenfd;
	//设置要处理的事件类型
	ev.events = EPOLLIN | EPOLLET;
	//注册 epoll 事件
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	//任务
	maxi = 0;
	while(1)
	{
		nfds = epoll_wait(epfd, events, 20, 500);
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == listenfd)
			{
				connfd = accept(listenfd, (sockaddr*)&clientaddr, &clilen);
				if(connfd < 0)
				{
					perror("accept");
					exit(1);
				}
				setnonblocking(connfd);
				printf("connet from ip:%s\n", inet_ntoa(clientaddr.sin_addr));
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}
			else if(events[i].events & EPOLLIN)	//读事件
			{
				printf("reading\n");
				if((sockfd = events[i].data.fd) < 0)
					continue;
				new_task = (struct task*)malloc(sizeof(struct task));
				new_task->fd = sockfd;
				new_task->next = NULL;
				pthread_mutex_lock(&mutex);
				if(readhead == NULL)
				{
					readhead = new_task;
					readtail = new_task;
				}else{
					readtail->next = new_task;
					readtail = new_task;
				}
				//唤醒所有等待cond1条件的子线程
				pthread_cond_broadcast(&cond1);
				pthread_mutex_unlock(&mutex);
			}
			else if(events[i].events & EPOLLOUT)	//写事件
			{
				rdata = (struct user_data *) events[i].data.ptr;
				sockfd = rdata->fd;
				write(sockfd, rdata->line, rdata->n_size);
				free(rdata);
				//设置用于读操作的文件描述符
				ev.data.fd = sockfd;
				//设置用于注测的读操作事件
				ev.events = EPOLLIN | EPOLLET;
				//修改 sockfd 上要处理的事件为 EPOLIN
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
}
