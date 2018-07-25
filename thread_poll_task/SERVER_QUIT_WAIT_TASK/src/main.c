#include "factory.h"
#define QUEUE_CAPACITY 100
#define TCP_CAPACITY 100
#define FILENAME "file"
int exit_fd[2];
void signal_exit(int signal_num)
{
	char exit_flag;
	if(SIGINT == signal_num){
		exit_flag = 'e';
	}
	write(exit_fd[1], &exit_flag, 1);
}

void *download_file(void *p)
{
	pfactory_t ptr_fac = (pfactory_t)p;
	pqueue_t task_q = ptr_fac->task_q;	//任务队列
	pnode_t current_task;	//当前线程工作
	while(1)
	{
		pthread_mutex_lock(&ptr_fac->mutex);
		while(0 == task_q->size && ptr_fac->work_flag)	//任务队列为空并且程序处于运行状态，等待
		{
			pthread_cond_wait(&ptr_fac->cond, &ptr_fac->mutex);
		}
		if(ptr_fac->work_flag == 0)	//停止工作
		{
			pthread_t tid = pthread_self();	//获取自己的线程id
			pthread_mutex_unlock(&ptr_fac->mutex);
			write(exit_fd[1], &tid, sizeof(pthread_t));	//发给主线程以供识别
			pthread_exit(NULL);
		}
		else
		{
			queue_pop(task_q, &current_task);
			pthread_mutex_unlock(&ptr_fac->mutex);
			if(current_task != NULL)
			{
				tran_file(current_task->data, FILENAME);
				free(current_task);	//不要在加解锁之间进行内核调用，free涉及内核访问，放在解锁之后
			}
		}
	}
}

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		printf("./SERVER ip, port, thread_num\n");
		return -1;
	}
	int ret_num;
	//注册退出信号
	pipe(exit_fd);
	signal(SIGINT, signal_exit);
	//启动工程
	factory_t factory;	//主要数据结构
	int thread_num = atoi(argv[3]);
	factory_init(&factory, download_file, thread_num);
	factory_start(&factory);
	//socket
	int socket_fd;
	ret_num = TCP_link_server(&socket_fd, argv[1], argv[2]);
	check_error(-1, ret_num, "TCP_link_server");
	//epoll
	int epfd = epoll_create(1);
	struct epoll_event event, ev[2];
	check_error(-1, epfd, "epoll_ctl");
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &event);
	check_error(-1, ret_num, "epoll_ctl");
	event.data.fd = exit_fd[0];
	ret_num = epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fd[0], &event);
	check_error(-1, ret_num, "epoll_ctl");
	//主要工作
	int new_fd;
	int read_fd_get;
	pnode_t new_task = NULL;	//接收新任务
	int i;
	while(1)
	{
		read_fd_get = epoll_wait(epfd, ev, 2, -1);	//监听
		for(i = 0; i < read_fd_get; i++)
		{
			if(socket_fd == ev[i].data.fd)	//取得连接
			{
				new_fd = accept(socket_fd, NULL, NULL);
				new_task = (pnode_t)calloc(1, sizeof(node_t));
				new_task->data = new_fd;
				pthread_mutex_lock(&factory.mutex);
				queue_insert(factory.task_q, new_task);	//向工作队列里增加工作
				pthread_mutex_unlock(&factory.mutex);
				pthread_cond_signal(&factory.cond);	//向子线程发送信号，可以开始工作
			}
			if(exit_fd[0] == ev[i].data.fd)	//检测到退出信号
			{
				close(socket_fd);	//关闭socket，不再接收新的网络请求
				char flag;
				ret_num = read(exit_fd[0], &flag, 1);
				check_error(-1, ret_num, "read_exit");
				if('e' == flag)
				{
					factory.work_flag = 0;	//设置停止工作，退出时，已经在传输的任务会继续完成，未开始的任务被弃置
					int j;
					pthread_t get_tid;
					for(j = 0; j < factory.th_num; j++)
					{
						pthread_cond_signal(&factory.cond);	//向每个挂起的子进程发送信号
						read(exit_fd[0], &get_tid, sizeof(pthread_t));	//读取子线程线程id
						pthread_join(get_tid, NULL);
					}
					queue_delete(factory.task_q);	//删除队列
					return 0;
				}
			}
		}
	}
	return 0;
}
