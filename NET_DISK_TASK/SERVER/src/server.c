#include "server.h"

int server_init(pserver_t server, thread_func_t thread_file_IO, thread_func_t thread_shell, int th_num)
{
	bzero(server, sizeof(server_t));
	server->th_id = (pthread_t*)calloc(th_num, sizeof(pthread_t));
	server->th_num = th_num;
	server->child_IO_queue = (pqueue_t)calloc(1, sizeof(queue_t));
	server->child_shell_queue = (pqueue_t)calloc(1, sizeof(queue_t));
	queue_init(server->child_IO_queue);
	queue_init(server->child_shell_queue);
	server->thread_file_IO = thread_file_IO;
	server->thread_shell = thread_shell;
#ifdef TCP_CAPACITY
	server->client_fd = (pclient_fd_t)calloc(TCP_CAPACITY, sizeof(client_fd_t));
	for(int j = 0; j < TCP_CAPACITY; j++)
	{
		server->client_fd[j].fd = -1;
		server->client_fd[j].work_flag = 0;
		//server->client_fd[j].dir_id = -1;
	}
#else
	server->client_fd = (pclient_fd_t)calloc(10, sizeof(client_fd_t));
	for(int j = 0; j < 10; j++)
	{
		server->client_fd[j].fd = -1;
		server->client_fd[j].work_flag = 0;
		//server->client_fd[j].dir_id = -1;
	}
#endif
	database_init(&server->database, "root", "loveistrue", "net_disk_project");
	database_login(&server->database);
	pthread_mutex_init(&server->database_lock, NULL);
	pthread_mutex_init(&server->log_lock, NULL);
	pthread_mutex_init(&server->log_lock, NULL);
	server->log_fd = open(LOG_PATH, O_RDWR|O_CREAT|O_EXCL, 0666);
	if(server->log_fd == -1 && errno == EEXIST)
	{
		server->log_fd = open(LOG_PATH, O_RDWR|O_APPEND, 0666);
	}
	return 1;
}

int server_start(pserver_t server)
{
	int i;
	int cut = server->th_num/4;	//1/4的线程进行shell指令
	if(cut == 0)
	{
		cut = 1;
	}
	for(i = 0; i < cut; i++)
	{
		pthread_create(server->th_id + i, NULL, server->thread_shell, server);
	}
	for(i = cut; i < server->th_num; i++)
	{
		pthread_create(server->th_id + i, NULL, server->thread_file_IO, server);
	}
	return 0;
}

int is_client_work(pclient_fd_t client, int fd)
{
	int j;
	for(j = 0; j < TCP_CAPACITY; j++)
	{
		if(client[j].fd == fd)
		{
			return (client[j].work_flag) ? 1 : 0;
		}
	}
	return -1;
}

int make_request(pTCP_protocol_t message, pnode_t task, int flag)
{
	message->order = task->work_id + flag;	//flag 为1 表示命令有效，让客户端发送配置信息，为0表示失败，客户端接收失败信息
	message->file_size = 0;
	//printf("request:%s\n", message->buffer);
	message->len = strlen(message->buffer);
	if(0 >= send_n(task->fd, (char*)message, TCP_PROTOCOL_SIZE))
	{
		return -1;
	}
	return 1;
}

void *thread_file_IO(void *p)
{
	//int ret_num;
	pserver_t server = (pserver_t)p;
	pqueue_t task_q = server->child_IO_queue;
	pnode_t current_task;	//用于从工作队列提取工作
	TCP_protocol_t message;	//用于传输
	//char buffer[100];
	while(1)
	{
		pthread_mutex_lock(&task_q->queue_lock);
		if(0 == task_q->size)   //任务队列为空，等待
		{
			pthread_cond_wait(&task_q->queue_not_empty, &task_q->queue_lock);
		}
		queue_pop(task_q, &current_task);   //取任务
		pthread_mutex_unlock(&task_q->queue_lock);
		if(NULL == current_task || -1 == current_task->work_id)  //是退出标记，先判断点是否为空，不为空再判断是否是退出符，不然会导致使用无效内存。
		{
			if(NULL != current_task)
			{
				free(current_task);
			}
			//printf("child thread over\n");
			pthread_exit(NULL);
		}
		else
		{
			//printf("current_task->option:%s\n", current_task->option);
			bzero(message.buffer, 1000);
			message.order = current_task->work_id;
			switch(current_task->work_id)
			{
				case 4:
					//printf("tran_file begin\n";
					if(0 >= do_task_gets(server, current_task, &message))
					{
						printf("server:send file fail\n");
					}
					else
					{
						printf("server:send file end\n");
					}
					break;
				case 5:
					//printf("wait to get file\n");
					//文件存在
					if(0 >= do_task_puts(server, current_task, &message))
					{
						printf("server:recv file fail\n");
					}
					else
					{
						printf("server:recv file succ\n");
					}
					break;
				default:
					break;
			}
			//log
			for(int j = 0; j < TCP_CAPACITY; j++)
			{
				if(server->client_fd[j].fd == current_task->fd)
				{
					server->client_fd[j].work_flag = 0;
					break;
				}
			}
			free(current_task);
		}
	}
}


void *thread_shell(void *p)
{
	//int ret_num;
	pserver_t server = (pserver_t)p;
	pqueue_t task_q = server->child_shell_queue;	//任务队列
	pnode_t current_task;	//用于从工作队列提取工作
	TCP_protocol_t message;
	while(1)
	{
		pthread_mutex_lock(&task_q->queue_lock);
		if(0 == task_q->size)	//任务队列为空，等待
		{
			pthread_cond_wait(&task_q->queue_not_empty, &task_q->queue_lock);
		}
		queue_pop(task_q, &current_task);	//取任务
		pthread_mutex_unlock(&task_q->queue_lock);
		if(NULL == current_task || -1 == current_task->work_id)	//是退出标记
		{
			if(NULL != current_task)
			{
				free(current_task);
			}
			//printf("child thread over\n");
			pthread_exit(NULL);	//线程结束
		}
		else
		{
			message.order = current_task->work_id;
			bzero(message.buffer, 1000);
			message.file_size = 0;
			//printf("begin work\n");
			switch(current_task->work_id)
			{
				case 0:
					do_task_cd(server, current_task, &message);
					break;
				case 1:
					do_task_ls(server, current_task, &message);
					break;
				case 2:
					do_task_remove(server, current_task, &message);
					break;
				case 3:
					do_task_pwd(server, current_task, &message);
					break;
				case COMMAND_LOGIN:
					do_task_login(server, current_task, &message);
					break;
				case COMMAND_REGISTER:
					do_task_regist(server, current_task, &message);
					break;
				default:
					break;
			}
			message.len = strlen(message.buffer);
			printf("server shell begin to send\n");
			if(0 >= send_n(current_task->fd, (char*)&message, TCP_PROTOCOL_SIZE))
			{
				printf("send_n error\n");
			}
			printf("server shell end to send\n");
			//log
			for(int j = 0; j < TCP_CAPACITY; j++)
			{
				if(server->client_fd[j].fd == current_task->fd)
				{
					server->client_fd[j].work_flag = 0;
					break;
				}
			}

			free(current_task);
		}
	}
}

void do_task_cd(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i = 0, j = 0, k;
	char name[20] = {0};
	char path[1000] = {0};
	char buffer[500] = {0};
	char dir_name[256] = {0};
	int pre_dir_id = 0;	//上一级文件id
	MYSQL_RES *result;
	MYSQL_ROW row;
	//找到目标目录
	//printf("%c\n", task->option[i]);
	if(task->option[i] == '\0' || task->option[i] == '\n')	//列出当前目录下文件
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				strcpy(path, server->client_fd[j].path);
				break;
			}
		}
	}
	else if(task->option[i] != '/')	//从当前目录开始寻找下面的目录
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				sprintf(path, "%s/%s", server->client_fd[j].path, task->option);
				break;
			}
		}
	}
	else	//从绝对目录开始寻找
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				break;
			}
		}
		strcpy(path, task->option);
	}
	bzero(server->log_buffer, LOG_BUFFER_SIZE);
	sprintf(server->log_buffer, "user:%s cd %s", name, task->option);
	log_input(server->log_fd, server->log_lock, server->log_buffer);
	//printf("%s\n", path);
	k = j;	//记录当前操作的客户成员序号
	j = 0;
	for(i = 1; path[i] != '\0' && (path[i] != '/' || path[i+1] != '\0'); i++)
	{
		if(path[i] == '/')
		{
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd' ", dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_name,dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row)	//未找到，目录无效
			{
				sprintf(message->buffer, "Invalid path");
				break;
			}
			else	//目录有效
			{
				pre_dir_id = atoi(row[1]);
				bzero(dir_name, sizeof(dir_name));
				j=0;
				mysql_free_result(result);
			}
		}
		else
		{
			dir_name[j++] = path[i];
		}
	}
	//确定最后一级目录的有效性
	if(path[i] == '\0' || (path[i] == '/' && path[i+1] == '\0'))
	{
		//获取当前目录id
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd'", dir_name, pre_dir_id);
		pthread_mutex_lock(&server->database_lock);
		database_select(&server->database, "dir_id", "FILE", buffer);
		result = mysql_use_result(server->database.connect_id);
		pthread_mutex_unlock(&server->database_lock);
		row = mysql_fetch_row(result);
		if(NULL == row)
		{
			sprintf(message->buffer, "Invalid path");
		}
		else	//路径正确
		{
			bzero(server->client_fd[k].path, 512);
			for(j = 0; path[j] != '\0'; j++)
			{
				;
			}
			j--;
			if(path[j] == '/')
			{
				path[j] = '\0';
			}
			strcpy(server->client_fd[k].path, path);
			sprintf(message->buffer,"go to %s", task->option);
		}
	}
	mysql_free_result(result);
}

void do_task_ls(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i = 0, j = 0;
	char name[20] = {0};
	char path[1000] = {0};
	char buffer[500] = {0};
	char dir_name[256] = {0};
	int pre_dir_id = 0;	//上一级文件id
	MYSQL_RES *result;
	MYSQL_ROW row;
	//找到目标目录
	//printf("%c\n", task->option[i]);
	if(task->option[i] == '\0' || task->option[i] == '\n')	//列出当前目录下文件
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				strcpy(path, server->client_fd[j].path);
				break;
			}
		}
	}
	else if(task->option[i] != '/')	//从当前目录开始寻找下面的目录
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				sprintf(path, "%s/%s", server->client_fd[j].path, task->option);
				break;
			}
		}
	}
	else	//从绝对目录开始寻找
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				break;
			}
		}
		strcpy(path, task->option);
	}
	bzero(server->log_buffer, LOG_BUFFER_SIZE);
	sprintf(server->log_buffer, "user:%s ls %s", name, task->option);
	log_input(server->log_fd, server->log_lock, server->log_buffer);
	//printf("%s\n", path);
	j = 0;
	for(i = 1; path[i] != '\0' && (path[i] != '/' || path[i+1] != '\0'); i++)
	{
		if(path[i] == '/')
		{
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd' ", dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_name,dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row)	//未找到，目录无效
			{
				sprintf(message->buffer, "Invalid path");
				break;
			}
			else	//目录有效
			{
				pre_dir_id = atoi(row[1]);
				bzero(dir_name, sizeof(dir_name));
				j=0;
				mysql_free_result(result);
			}
		}
		else
		{
			dir_name[j++] = path[i];
		}
	}
	//列出当前用户目录下的所有文件
	if(path[i] == '\0' || (path[i] == '/' && path[i+1] == '\0'))
	{
		//获取当前目录id
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd'", dir_name, pre_dir_id);
		pthread_mutex_lock(&server->database_lock);
		database_select(&server->database, "dir_id", "FILE", buffer);
		result = mysql_use_result(server->database.connect_id);
		pthread_mutex_unlock(&server->database_lock);
		row = mysql_fetch_row(result);
		if(NULL == row)
		{
			sprintf(message->buffer, "Invalid path");
		}
		else
		{
			pre_dir_id = atoi(row[0]);
			mysql_free_result(result);
			bzero(buffer, sizeof(buffer));
			//sprintf(buffer,"name='%s' and pre_dir_id=%d", name, pre_dir_id);
			sprintf(buffer,"pre_dir_id=%d", pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_type,file_name,file_size", "FILE", buffer);	//文件类型，名字，大小
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			while(NULL != row)
			{
				bzero(buffer, sizeof(buffer));
				sprintf(buffer,"%s %-20s %-20s\n", row[0], row[1], row[2]);
				strcat(message->buffer, buffer);
				row = mysql_fetch_row(result);
			}
		}
	}
	mysql_free_result(result);
}


void do_task_remove(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i = 0, j = 0;
	char name[20] = {0};
	char path[1000] = {0};
	char buffer[500] = {0};
	char dir_name[256] = {0};
	int pre_dir_id = 0;	//上一级文件id
	MYSQL_RES *result;
	MYSQL_ROW row;
	//找到目标目录
	//printf("%c\n", task->option[i]);
	if(task->option[i] == '\0' || task->option[i] == '\n')	//列出当前目录下文件
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				strcpy(path, server->client_fd[j].path);
				break;
			}
		}
	}
	else if(task->option[i] != '/')	//从当前目录开始寻找下面的目录
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				sprintf(path, "%s/%s", server->client_fd[j].path, task->option);
				break;
			}
		}
	}
	else	//从绝对目录开始寻找
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				break;
			}
		}
		strcpy(path, task->option);
	}
	bzero(server->log_buffer, LOG_BUFFER_SIZE);
	sprintf(server->log_buffer, "user:%s remove %s", name, task->option);
	log_input(server->log_fd, server->log_lock, server->log_buffer);
	//printf("%s\n", path);
	j = 0;
	for(i = 1; path[i] != '\0'; i++)
	{
		if(path[i] == '/')
		{
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd' ", dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_name,dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row)	//未找到，目录无效
			{
				sprintf(message->buffer, "Path not points to a file");
				break;
			}
			else	//目录有效
			{
				pre_dir_id = atoi(row[1]);
				bzero(dir_name, sizeof(dir_name));
				j=0;
				mysql_free_result(result);
			}
		}
		else
		{
			dir_name[j++] = path[i];
		}
	}
	//找出当前目录下是否存在该文件
	if(path[i] == '\0')
	{
		if(dir_name[0] == '\0')
		{
			sprintf(message->buffer, "Path not points to a file");
		}
		else
		{
			//删除文件
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"name='%s' and file_name='%s' and pre_dir_id=%d and file_type != 'd'", name, dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row)
			{
				sprintf(message->buffer, "File not exist, remove failure");
				mysql_free_result(result);
			}
			else	//执行删除
			{
				pre_dir_id = atoi(row[0]);
				mysql_free_result(result);
				bzero(buffer, sizeof(buffer));
				//sprintf(buffer,"name='%s' and pre_dir_id=%d", name, pre_dir_id);
				sprintf(buffer,"dir_id=%d", pre_dir_id);
				pthread_mutex_lock(&server->database_lock);
				database_delete(&server->database, "FILE", buffer);
				pthread_mutex_unlock(&server->database_lock);
				sprintf(message->buffer, "remove success");
			}
		}
	}
}

void do_task_pwd(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	for(int j = 0; j < TCP_CAPACITY; j++)
	{
		if(server->client_fd[j].fd == task->fd)
		{
			bzero(server->log_buffer, LOG_BUFFER_SIZE);
			sprintf(server->log_buffer, "user:%s pwd", server->client_fd[j].name);
			log_input(server->log_fd, server->log_lock, server->log_buffer);
			sprintf(message->buffer, "%s", server->client_fd[j].path);
			break;
		}
	}
}

void do_task_login(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i, j;
	char name[20] = {0};
	char code[20] = {0};
	char buffer[200] = {0};
	MYSQL_ROW row;
	MYSQL_RES *result;
	//取得用户输入的名字和密码
	for(i = 0; task->option[i] != '$'; i++)
	{
		name[i] = task->option[i];
	}
	i++;
	for(j = 0; task->option[i] != '\0'; i++, j++)
	{
		code[j] = task->option[i];
	}
	printf("get use name:%s\n", name);
	sprintf(buffer, "name='%s'", name);
	pthread_mutex_lock(&server->database_lock);
	database_select(&server->database, "code,salt_code,path", "USER", buffer);
	//printf("select ok\n");
	result = mysql_use_result(server->database.connect_id);
	pthread_mutex_unlock(&server->database_lock);
	row = mysql_fetch_row(result);
	//printf("%s\n%s\n%s\n", row[0], row[1], row[2]);
	if(NULL != row)	//查到结果，姓名存在，姓名是主键，结果必须为1
	{
		if(!strcmp(row[0], crypt(code, row[1])))	//密码比对成功
		{
			strcpy(message->buffer, "success");
			for(j = 0; j < TCP_CAPACITY; j++)
			{
				if(server->client_fd[j].fd == task->fd)
				{
					strcpy(server->client_fd[j].name, name);
					strcpy(server->client_fd[j].path, row[2]);
					//server->client_fd[j].dir_id = 1;
					bzero(server->log_buffer, LOG_BUFFER_SIZE);
					sprintf(server->log_buffer, "user:%s login", name);
					log_input(server->log_fd, server->log_lock, server->log_buffer);
					break;
				}
			}
		}
		else{
			strcpy(message->buffer, "login failure");
		}
	}else{	//结果为空
		strcpy(message->buffer, "login failure");
	}
	mysql_free_result(result);
	//printf("%s\n", message->buffer);
}


void get_rand_str(char *buffer, int num)    //生成随即字符串
{
	//定义随机生成字符串表
	char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
	int i,lstr;
	char ss;
	lstr = strlen(str);//计算字符串长度
	srand((unsigned int)time((time_t *)NULL));//使用系统时间来初始化随机数发生器
	for(i = 0; i < num; i++)
	{
		ss = str[(rand()%lstr)];
		buffer[i] = ss;
	}
}


void do_task_regist(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i, j;
	char name[20] = {0};
	char code[20] = {0};
	char buffer[500] = {0};
	char rand_str[8] = {0};
	char salt_code[11] = {0};
	char path[30] = {0};
	char *p_code;
	MYSQL_RES *result;
	MYSQL_ROW row;
	//取得用户输入的名字和密码
	for(i = 0; task->option[i] != '$'; i++)
	{
		name[i] = task->option[i];
	}
	i++;
	for(j = 0; task->option[i] != '\0'; i++, j++)
	{
		code[j] = task->option[i];
	}
	sprintf(buffer, "name = '%s'", name);
	pthread_mutex_lock(&server->database_lock);
	database_select(&server->database, "name", "USER", buffer);
	result = mysql_use_result(server->database.connect_id);
	pthread_mutex_unlock(&server->database_lock);
	row = mysql_fetch_row(result);
	if(NULL == row)	//没查到结果，姓名不存在，可以创建
	{
		mysql_free_result(result);
		strcpy(message->buffer, "success");
		sprintf(path, "/root/%s", name);
		//生成8位MD5盐码
		get_rand_str(rand_str, 8);
		sprintf(salt_code, "%s%s", "$6$" , rand_str);
		//根据用户密码生成完整密码
		p_code = crypt(code, salt_code);
		//数据库插入准备
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "'%s','%s','%s','%s'",
				name, salt_code, p_code, path);
		//数据库操作开始
		pthread_mutex_lock(&server->database_lock);
		//创建新的用户USER
		database_add(&server->database, "USER", "name,salt_code,code,path", buffer);
		//创建新的文件FILE 用户目录
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "0,'%s','%s','d'", name, name);
		database_add(&server->database, "FILE", "pre_dir_id,name,file_name,file_type", buffer);
		pthread_mutex_unlock(&server->database_lock);
		//数据库操作结束
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(server->client_fd[j].name, name);
				sprintf(server->client_fd[j].path, "%s", path);	//为新的用户以姓名为目录名字，创建目录
				bzero(server->log_buffer, LOG_BUFFER_SIZE);
				sprintf(server->log_buffer, "user:%s regist", name);
				log_input(server->log_fd, server->log_lock, server->log_buffer);
				//server->client_fd[j].dir_id = atoi(row[0]);
				break;
			}
		}
	}else{	//结果不为空，名字被使用
		strcpy(message->buffer, "regist failure");
		mysql_free_result(result);
	}
}

int do_task_gets(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i = 0, j = 0;
	char name[20] = {0};
	char path[512] = {0};
	char buffer[500] = {0};
	char dir_name[256] = {0};
	int pre_dir_id = 0; //上一级文件id
	MYSQL_RES *result;
	MYSQL_ROW row;
	//检验文件路径
	if(task->option[i] != '/') //从当前目录开始寻找下面的目录
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				sprintf(path, "%s/%s", server->client_fd[j].path, task->option);
				break;
			}
		}
	}
	else    //从绝对目录开始寻找
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				break;
			}
		}
		strcpy(path, task->option);
	}
	bzero(server->log_buffer, LOG_BUFFER_SIZE);
	sprintf(server->log_buffer, "user:%s gets %s", name, task->option);
	log_input(server->log_fd, server->log_lock, server->log_buffer);
	j = 0;
	for(i = 1; path[i] != '\0'; i++)
	{
		if(path[i] == '/')
		{
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd' ", dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_name,dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row) //未找到，目录无效
			{
				sprintf(message->buffer, "Path not points to a file");
				break;
			}
			else    //目录有效
			{
				pre_dir_id = atoi(row[1]);
				bzero(dir_name, sizeof(dir_name));
				j=0;
				mysql_free_result(result);
			}
		}
		else
		{
			dir_name[j++] = path[i];
		}
	}
	//找出当前目录下是否存在该文件
	if(path[i] == '\0')
	{
		if(dir_name[0] == '\0')
		{
			sprintf(message->buffer, "Path not points to a file");
		}
		else
		{
			//在当前目录找目标文件
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"name='%s' and file_name='%s' and pre_dir_id=%d and file_type != 'd'", name, dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "md5", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row)
			{
				sprintf(message->buffer, "File not exist, gets failure");
				mysql_free_result(result);
			}
			else    //找到，获取文件md5码
			{
				sprintf(buffer, "%s", row[0]);  //取得文件MD5码
				mysql_free_result(result);
				sprintf(message->buffer, "%s", dir_name);   //准备发送文件名
				message->order += 1;
			}
		}
	}
	message->len = strlen(message->buffer);
	//buffer记录了文件MD5码
	if(message->order - 1 == task->work_id) //证明找到文件
	{
		bzero(path, sizeof(path));
		sprintf(path, "%s/%s", USER_FILE_DIR, buffer);
		if(0 >= send_file(task->fd, path, message))
		{
			return -1;
		}
	}
	else
	{
		if(0 >= send_n(task->fd, (char*)message, TCP_PROTOCOL_SIZE))
		{
			printf("send error\n");
			return -1;
		}
	}
	return 1;
}

int do_task_puts(pserver_t server , pnode_t task, pTCP_protocol_t message)
{
	int i = 0, j = 0;
	char name[20] = {0};
	char path[512] = {0};
	char buffer[500] = {0};
	char dir_name[256] = {0};
	char md5_code[33] = {0};
	int pre_dir_id = 0; //上一级文件id
	MYSQL_RES *result;
	MYSQL_ROW row;
	/*提取MD5码*/
	//printf("%s\n", task->option);
	while(task->option[i] != '$' && i < 512)
	{
		i++;
	}
	task->option[i] = '\0';
	i++;
	while(task->option[i] != '\0' && j < 32)
	{
		md5_code[j++] = task->option[i];
		task->option[i++] = '\0';
	}
	//printf("%s\n", md5_code);
	i = 0; j = 0;
	/*检验文件路径 判断文件是否存在*/
	if(task->option[i] != '/') //从当前目录开始寻找下面的目录
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				sprintf(path, "%s/%s", server->client_fd[j].path, task->option);
				break;
			}
		}
	}
	else    //从绝对目录开始寻找
	{
		for(j = 0; j < TCP_CAPACITY; j++)
		{
			if(server->client_fd[j].fd == task->fd)
			{
				strcpy(name, server->client_fd[j].name);
				break;
			}
		}
		strcpy(path, task->option);
	}
	bzero(server->log_buffer, LOG_BUFFER_SIZE);
	sprintf(server->log_buffer, "user:%s gets %s", name, task->option);
	log_input(server->log_fd, server->log_lock, server->log_buffer);
	j = 0;
	for(i = 1; path[i] != '\0'; i++)
	{
		if(path[i] == '/')
		{
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"file_name='%s' and pre_dir_id=%d and file_type = 'd' ", dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "file_name,dir_id", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row) //未找到，目录无效
			{
				sprintf(message->buffer, "Path not points to a file");
				break;
			}
			else    //目录有效
			{
				pre_dir_id = atoi(row[1]);
				bzero(dir_name, sizeof(dir_name));
				j=0;
				mysql_free_result(result);
			}
		}
		else
		{
			dir_name[j++] = path[i];
		}
	}
	/*找出当前目录下是否存在该文件*/
	if(path[i] == '\0')
	{
		if(dir_name[0] == '\0')
		{
			sprintf(message->buffer, "Path not points to a file");
		}
		else
		{
			/*在当前目录找目标文件*/
			bzero(buffer, sizeof(buffer));
			sprintf(buffer,"name='%s' and file_name='%s' and pre_dir_id=%d and file_type != 'd'", name, dir_name, pre_dir_id);
			pthread_mutex_lock(&server->database_lock);
			database_select(&server->database, "md5", "FILE", buffer);
			result = mysql_use_result(server->database.connect_id);
			pthread_mutex_unlock(&server->database_lock);
			row = mysql_fetch_row(result);
			if(NULL == row) //未找到，说明文件不存在
			{
				sprintf(message->buffer, "%s", dir_name);   //准备发送文件名
				message->order += 1;
				mysql_free_result(result);
			}
			else    //找到，说明文件已经存在于用户名下
			{
				mysql_free_result(result);
				sprintf(message->buffer, "File exist, no need to puts");
			}
		}
	}
	message->len = strlen(message->buffer);
	/*md5_code记录了文件MD5码*/
	if(message->order - 1 == task->work_id) //证明找到文件
	{
		bzero(path, sizeof(path));
		//printf("%s\n", md5_code);
		sprintf(path, "%s/%s", USER_FILE_DIR, md5_code);
		//printf("%s\n", path);
		if(0 >= recv_file(task->fd, path, message))
		{
			printf("server get file failure\n");
		}
		/*在数据库里添加记录*/
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "'%s','%s','f',%d,'%s',%ld", name, dir_name, pre_dir_id, md5_code, message->file_size);
		//printf("%s\n", buffer);
		pthread_mutex_lock(&server->database_lock);
		database_add(&server->database, "FILE", "name,file_name,file_type,pre_dir_id,md5,file_size", buffer);
		pthread_mutex_unlock(&server->database_lock);
		printf("server get file\n");
	}
	else
	{
		if(0 >= send_n(task->fd, (char*)message, TCP_PROTOCOL_SIZE))
		{
			printf("send error\n");
			return -1;
		}
	}
	return 1;
}

int send_file(int new_fd, char *filepath, pTCP_protocol_t message)
{
	int ret_num;
	struct stat f_stat;
	stat(filepath, &f_stat);
	message->file_size = f_stat.st_size;	//文件大小
	int fd = open(filepath, O_RDONLY);
	check_error(-1, fd, "open");
	//先发送文件名、文件大小
	ret_num = send_n(new_fd, (char*)message, TCP_PROTOCOL_SIZE);
	check_IO(ret_num);
	//接收客户端发来的文件大小
	ret_num = recv_n(new_fd, (char*)message, TCP_PROTOCOL_SIZE);
	check_IO(ret_num);
	if(f_stat.st_size <= message->file_size)	//客户端已经有文件
	{
		close(fd);
		return -1;
	}
	ret_num = sendfile(new_fd, fd, &message->file_size, f_stat.st_size);
	check_IO(ret_num);
	/*
	   if(sendfile(new_fd, fd, 0, f_stat.st_size)<0)
	   {
	//printf("%d\n", errno);
	//printf("sendfile error\n");
	close(new_fd);
	return -1;
	}
	*/
	//printf("sendfile ok\n");
	close(fd);
	return 1;
}

int recv_file(int new_fd, char *filepath, pTCP_protocol_t message)
{
	double total_download = 0;
	int ret_num = 0;
	char buffer[1000];
	off_t file_size;
	struct stat f_stat;
	//time_t start, end;
	//start = time(NULL);
	int fd = open(filepath, O_CREAT|O_RDWR|O_EXCL, 0666);
	if(fd == -1 && errno == EEXIST)	//如果文件存在
	{
		fd = open(filepath, O_RDWR|O_APPEND, 0666);
		check_error(-1, fd, "open");
		stat(filepath, &f_stat);
		message->file_size = f_stat.st_size;
	}
	else
	{
		f_stat.st_size = 0;
		message->file_size = 0;
	}
	ret_num = send_n(new_fd, (char*)message, TCP_PROTOCOL_SIZE);
	check_IO(ret_num);
	ret_num = recv_n(new_fd, (char*)message, TCP_PROTOCOL_SIZE);
	check_IO(ret_num);
	file_size = message->file_size;
	if(f_stat.st_size >= file_size)
	{
		close(fd);
		return -1;
	}
	total_download = f_stat.st_size;	//记录文件开始记录点
	//收文件
	while(total_download < file_size)
	{
		bzero(buffer, 1000);
		ret_num = recv(new_fd, buffer, 1000, 0);
		if(0 > ret_num)
		{
			if(errno == EINTR || errno == EAGAIN)
			{
				ret_num = 0;
			}else{
				return -1;
			}
		}else if(ret_num == 0){
			return 0;
		}
		/*
		   if(ret_num == -1)
		   {
		   printf("recv_file error in func\n");
		   return -1;
		   }
		   else
		   {
		   if(ret_num == 0)
		   {
		   printf("client close\n");
		   return -1;
		   }
		   total_download += ret_num;
		   end = time(NULL);
		   if(end - start > 1)
		   {
		   printf("download percent %5.2f%c\r", total_download/size_file*100, '%');
		   fflush(stdout);
		   start = end;
		   }
		   write(fd, buffer, ret_num);
		   }
		   */
		total_download += ret_num;
		write(fd, buffer, ret_num);
	}
	//printf("download percent %5.2f%c\n", total_download/size_file*100, '%');
	close(fd);
	return 1;
}

void log_input(int log_fd, pthread_mutex_t log_lock, char *log_buffer)
{
	time_t log_time;
	struct tm *log_gtime;
	char buffer[230];
	time(&log_time);
	log_gtime = gmtime(&log_time);
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d %s\n",
			(1900 + log_gtime->tm_year),(1 + log_gtime->tm_mon),(log_gtime->tm_mday),(8 + log_gtime->tm_hour),(log_gtime->tm_min),(log_gtime->tm_sec),
			log_buffer);
	pthread_mutex_lock(&log_lock);
	write(log_fd, buffer, strlen(buffer));
	pthread_mutex_unlock(&log_lock);
}
