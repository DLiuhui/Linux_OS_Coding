#ifndef __SERVER_H__
#define __SERVER_H__

#include "header.h"
#include "r_s_api.h"
#include "work_queue.h"
#include "database_api.h"

#define COMMAND_ORDER_SIZE 20
#define COMMAND_OPTION_SIZE 512
#define TCP_PROTOCOL_SIZE 1016
#define LOG_BUFFER_SIZE 200

#define COMMAND_LOGIN 100
#define COMMAND_REGISTER 101

#define USER_FILE_DIR "../../USER_FILES"	//统一的用户文件存储目录

#define LOG_PATH "../log"

typedef void* (*thread_func_t)(void*);

typedef struct client_fd
{
	int fd;
	int work_flag;
	//int dir_id;	//目录路径id
	char name[20];	//记录用户名
	char path[512];	//记录路径
}client_fd_t, *pclient_fd_t;

typedef struct TCP_protocol
{
	int len;    //报文buffer长度4
	int order;	//命令id 4
	long int file_size;    //如果传文件，文件大小8
	char buffer[1000];  //报文内容 1000
}TCP_protocol_t, *pTCP_protocol_t;

typedef struct server
{
	pthread_t *th_id;	//子线程id
	int th_num;	//子线程数目
	pqueue_t child_IO_queue;	//子线程的工作队列
	pqueue_t child_shell_queue;
	thread_func_t thread_file_IO;	//文件传输线程
	thread_func_t thread_shell;	//shell指令线程
	pclient_fd_t client_fd;	//客户端句柄
	database_t database;	//用于操作数据库
	pthread_mutex_t database_lock;	//数据库锁，保证数据库操作原子性
	int log_fd;	//日志用句柄
	pthread_mutex_t log_lock; //日志锁，保证日志记录的原子性
	char log_buffer[LOG_BUFFER_SIZE];	//日志buffer
}server_t, *pserver_t;

//初始化服务器
int server_init(pserver_t, thread_func_t, thread_func_t, int);

//启动服务器
int server_start(pserver_t);

int is_client_work(pclient_fd_t, int);

int make_request(pTCP_protocol_t, pnode_t, int);

void *thread_file_IO(void *p);

void *thread_shell(void *p);

void do_task_cd(pserver_t, pnode_t, pTCP_protocol_t);

void do_task_ls(pserver_t, pnode_t, pTCP_protocol_t);

void do_task_remove(pserver_t, pnode_t, pTCP_protocol_t);

void do_task_pwd(pserver_t, pnode_t, pTCP_protocol_t);

void do_task_login(pserver_t, pnode_t, pTCP_protocol_t);

void do_task_regist(pserver_t, pnode_t, pTCP_protocol_t);

int do_task_gets(pserver_t, pnode_t, pTCP_protocol_t);

int do_task_puts(pserver_t, pnode_t, pTCP_protocol_t);

int send_file(int, char*, pTCP_protocol_t);

int recv_file(int, char*, pTCP_protocol_t);

void log_input(int, pthread_mutex_t, char*);
#endif
