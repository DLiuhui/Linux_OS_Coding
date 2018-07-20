#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "header.h"
#include "r_s_api.h"

#define COMMAND_ORDER_SIZE 20
#define COMMAND_OPTION_SIZE 512
#define TCP_PROTOCOL_SIZE 1016

#define COMMAND_LOGIN 100
#define COMMAND_REGISTER 101

#define READ_MAX_SIZE 1024
#define MD5_SIZE 16
#define MD5_STR_LEN (MD5_SIZE * 2)

typedef struct TCP_protocol
{
	int len;	//报文长度4
	int order;	//命令id 4
	long int file_size;	//如果传文件，文件大小8
	char buffer[1000];	//报文内容 1000
}TCP_protocol_t, *pTCP_protocol_t;

typedef struct client_command
{
	int id; //生成命令的id
	char option[COMMAND_OPTION_SIZE];   //读取用户指令选项
	/*char order[COMMAND_ORDER_SIZE] //存储用户指令*/
}client_command_t, *pclient_command_t;

typedef struct user_message
{
	char user_name[20];	//记录用户名
	//char user_path[512];	//记录用户路径
}user_message_t, *puser_message_t;

int get_input(char*, char*, int, char);	//获取标准输入

int get_command_id(pclient_command_t, char*, const char **, const int);	//获取命令id

int get_command_option(pclient_command_t, char*);	//命令有效则获取命令选项

int get_user_order(pclient_command_t, char*, const char**, const int);	//用户命令输入处理接口

int do_user_task(int, pclient_command_t);

int user_login(int, puser_message_t);	//用户登录操作

int send_file(int, char*, pTCP_protocol_t);

int recv_file(int, char*, pTCP_protocol_t);

int get_md5(char *, char*);
#endif
