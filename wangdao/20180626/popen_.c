#include "header.h"
#define BUFFER_SIZE 512
//打开通信管道

int main()
{
	FILE *read_fp;
	char buffer[BUFFER_SIZE];
	int get_read;
	memset(buffer, '\0', sizeof(buffer));	//初始化buffer
	read_fp = popen("ps -ax", "r");
	if(read_fp != NULL)
	{
		get_read = fread(buffer, sizeof(char), BUFFER_SIZE, read_fp);
		while(get_read > 0)
		{
			buffer[get_read - 1] = '\0';
			printf("Reading: %s\n", buffer);
			get_read = fread(buffer, sizeof(char), BUFFER_SIZE, read_fp);
		}
		pclose(read_fp);
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
}
