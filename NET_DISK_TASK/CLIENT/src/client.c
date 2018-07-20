#include "client.h"

int get_input(char *order, char *buffer, int buffer_size, char key)
{
	int i = 0, j = 0;
	while(buffer[j] == '\0' || buffer[j] == ' ')
	{
		j++;
	}
	while(buffer[j] != key &&  buffer[j] != '\0' && buffer[j] != '\n')
	{
		if(i < buffer_size)
		{
			order[i++] = buffer[j];
			buffer[j++] = '\0';
		}
	}
	buffer[j] = '\0';
	(i < buffer_size) && (order[i] = '\0');
	return 1;
}

int get_command_id(pclient_command_t client, char *command, const char **command_list, const int command_num)
{
	int i = 0;
	client->id = -1;
	for(i = 0; i < command_num; i++)
	{
		if(!strcmp(command, command_list[i]))
		{
			client->id = i;
			break;
		}
	}
	//printf("%d\n", client->id);
	if(-1 == client->id) //命令匹配失败
	{
		return -1;
	}
	return 1;
}

int get_command_option(pclient_command_t client, char *buffer)
{
	if(3 == client->id) //是pwd
	{
		client->option[0] = '\0';
		return 1;
	}
	get_input(client->option, buffer, COMMAND_OPTION_SIZE, 0);
	return 1;
}

int get_user_order(pclient_command_t client, char *buffer, const char **command_list, const int command_list_size)
{
	char order[COMMAND_ORDER_SIZE];
	//printf("begin get_user_order\n");
	get_input(order, buffer, COMMAND_ORDER_SIZE, ' ');
	//printf("get_user_order:%s\n", order);
	if(-1 == get_command_id(client, order, command_list, command_list_size))
	{
		return -1;
	}
	get_command_option(client, buffer);
	//printf("get_user_order:%d\n", client->id);
	return 1;
}

int do_user_task(int socket_fd, pclient_command_t client)
{
	TCP_protocol_t message;
	char file_path[512] = {0};
	char md5_str[32] = {0};
	struct stat f_stat;
	bzero(message.buffer, 1000);
	message.order = client->id;
	strcpy(message.buffer, client->option);
	if(client->id == 5)
	{
		//printf("%s\n", client->option);
		int fd = open(client->option, O_RDWR);
		//printf("%d,%s\n", fd,client->option);
		if(0 > fd)
		{
			printf("Invalid file path\n");
			return -1;
		}
		else
		{
			//获取文件md5码
			close(fd);
			get_md5(client->option, md5_str);
			stat(client->option, &f_stat);
			//printf("%s\n", md5_str);
			strcat(message.buffer, "$");
			strcat(message.buffer, md5_str);
		}
	}
	printf("%s\n", message.buffer);
	message.file_size = 0;
	//向服务器发送协议
	//printf("client begin to send:%s\n", message.buffer);
	int ret_num = send_n(socket_fd, (char*)&message, TCP_PROTOCOL_SIZE);
	//printf("client end send\n");
	check_IO(ret_num);
	//printf("client begin to recv\n");
	ret_num = recv_n(socket_fd, (char*)&message, TCP_PROTOCOL_SIZE);	//接收客户端回复
	//printf("client end recv\n");
	check_IO(ret_num);
	if(client->id == 4 || client->id == 5)
	{
		if(message.order - client->id == 1)
		{
			if(client->id == 4)	//gets
			{
				printf("client begin to recv file_name\n");
				sprintf(file_path, "./%s", message.buffer);
				//printf("client end to recv file_name\n");
				recv_file(socket_fd, file_path, &message);
				printf("client end to recv file\n");
				check_IO(ret_num);
			}
			else	//puts
			{
				printf("client begin to send file:%s\nplease wait\n", client->option);
				ret_num = send_file(socket_fd, client->option, &message);
				printf("client end to send file\n");
				check_IO(ret_num);
			}
		}
		else
		{
			printf("%s\n", message.buffer);
		}
	}
	else
	{
		printf("%s\n", message.buffer);
	}
	return 1;
}

int user_login(int socket_fd, puser_message_t user)
{
	TCP_protocol_t message;
	bzero(message.buffer, 1000);
	bzero(user->user_name, 20);
	char code[20] = {0};
	char c;	//选项
	int i;
	printf("Welcome!\n");
	while(1)
	{
		printf("choose 1 to login, 2 to regist:\n");
		while((c = getchar()) == '\n' || c == EOF) {;}
		if(c == '1')
		{
			message.order = COMMAND_LOGIN;
			clear_out_stdin(c);
			i = 0;
			printf("printf name:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				user->user_name[i++] = c;
			}
			i = 0;
			printf("printf code:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				code[i++] = c;
			}
			if('\0' == user->user_name[0] || '\0' == code[0])
			{
				printf("name or code couldn't be empty\n");
				continue;
			}
		}
		else if(c == '2')
		{
			message.order = COMMAND_REGISTER;
			clear_out_stdin(c);
			i = 0;
			printf("printf new name:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				user->user_name[i++] = c;
			}
			i = 0;
			printf("printf new code:");
			while((c = getchar()) != '\n' && c != EOF && i < 20)
			{
				code[i++] = c;
			}
			if('\0' == user->user_name[0] || '\0' == code[0])
			{
				printf("name or code couldn't be empty\n");
				continue;
			}
		}
		else
		{
			clear_out_stdin(c);
			printf("select error\n");
			continue;
		}
		message.file_size = 0;
		sprintf(message.buffer, "%s$%s", user->user_name, code);
		message.len = strlen(message.buffer);
		if(0 >= send_n(socket_fd, (char*)&message, TCP_PROTOCOL_SIZE))	//向服务器发送信息
		{
			return -1;
		}
		if(0 >= recv_n(socket_fd, (char*)&message, TCP_PROTOCOL_SIZE))	//接收服务器的回复
		{
			return -1;
		}
		if(message.order == COMMAND_LOGIN)
		{
			if(!strcmp(message.buffer, "success"))	//登录成功
			{
				printf("login success\n");
				return 1;
			}else{
				printf("wrong user name or id!\n");
				continue;
			}
		}
		else if(message.order == COMMAND_REGISTER)
		{
			if(!strcmp(message.buffer, "success"))	//注册成功
			{
				printf("register success\n");
				return 1;
			}else{
				printf("your user name has been used, try again\n");
				continue;
			}
		}
	}
}


int send_file(int new_fd, char *filepath, pTCP_protocol_t message)
{
	int ret_num;
	struct stat f_stat;
	//printf("%s\n", filepath);
	stat(filepath, &f_stat);
	off_t file_size = message->file_size;	//客户端文件大小
	message->file_size = f_stat.st_size;	//发送本地文件大小
	ret_num = send_n(new_fd, (char*)message, TCP_PROTOCOL_SIZE);
	check_IO(ret_num);
	printf("file_size: %ld\n", file_size);
	if(f_stat.st_size <= file_size)	//本地文件大小小于客户端
	{
		return -1;
	}
	//printf("file name send\n");
	//sendfile，发送文件内容
	int fd = open(filepath, O_RDONLY);
	check_error(-1, fd, "open");
	ret_num = sendfile(new_fd, fd, &file_size, f_stat.st_size);
	check_IO(ret_num);
	//printf("%d\n", errno);
	//printf("sendfile error\n");
	//close(new_fd);
	//return -1;
	printf("sendfile ok\n");
	//close(new_fd);
	return 1;
}

int recv_file(int new_fd, char *filepath, pTCP_protocol_t message)
{
	double total_download = 0;
	off_t file_size;
	int ret_num = 0;
	char buffer[1000];
	time_t start, end;
	start = time(NULL);
	struct stat f_stat;
	file_size = message->file_size;	//待接收文件大小
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
	if(f_stat.st_size >= file_size)
	{
		return -1;
	}
	total_download = f_stat.st_size;	//记录文件开始点
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
		//printf("client close\n");
		return 0;
		}
		}
		*/
		total_download += ret_num;
		end = time(NULL);
		if(end - start > 1)
		{
			printf("download percent %5.2f%c\r", total_download/file_size*100, '%');
			fflush(stdout);
			start = end;
		}
		write(fd, buffer, ret_num);
	}
	printf("download percent %5.2f%c\n", total_download/file_size*100, '%');
	return 1;
}

int get_md5(char *path, char *md5_str)
{
	int fd;
	int ret = 0;
	int i = 0;
	unsigned char data[READ_MAX_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;
	//printf("%s\n", path);
	if ((fd = open(path, O_RDWR)) < 0)
	{
		perror("open failed");
		return -1;
	}
	MD5_Init(&md5);

	while(1)
	{
		ret = read(fd, data, READ_MAX_SIZE);
		if (ret == -1)
		{
			perror("read failed");
			close(fd);
			return -1;
		}
		MD5_Update(&md5, data, ret);
		if (ret == 0 || ret < READ_MAX_SIZE)
		{
			break;
		}
	}
	close(fd);
	MD5_Final((unsigned char *)&md5_value, &md5);
	for (i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i * 2, 2 + 1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0';
	return 0;
}
