#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 20
const char *command_list[] = {"cd", "ls", "remove", "pwd", "gets", "puts"};
#define COMMAND_NUM (sizeof(command_list)/sizeof(char*))
typedef struct client_command
{
	int id;	//生成命令的id
	/*char orders[10];	//读取用户指令*/
	char option[256];	//读取用户指令选项
}client_command_t, *pclient_command_t;

int get_input(char *buf, int buffer_size, char key)
{
	char c;
	int i = 0;
	while((c = getc(stdin)) != key && c != '\n' && c != EOF)
	{
		if(i < buffer_size)
		{
			buf[i++] = c;
		}else{
			break;
		}
	}
	(i < buffer_size) && (buf[i] = '\0');
	return 1;
}

int get_command_id(int *c_id, char *command)
{
	int i = 0;
	*c_id = -1;
	for(i = 0; i < COMMAND_NUM; i++)
	{
		if(!strcmp(command, command_list[i]))
		{
			*c_id = i;
			break;
		}
	}
	if(-1 == *c_id)	//命令匹配失败
	{
		return -1;
	}
	return 1;
}
int get_command_option(pclient_command_t client)
{
	if(client->id == 3)	//不是pwd
	{
		client->option[0] = '\0';
		return 1;
	}
	if(-1 == get_input(client->option, 256, 0))
		return -1;
	return 1;
}
int main()
{
	client_command_t client;
	char p[10];
	get_input(p, 10, ' ');
	get_command_id(&client.id, p);
	printf("%s\n", p);
	printf("%d\n", client.id);
	get_command_option(&client);
	printf("%s\n", client.option);
	return 0;
}
