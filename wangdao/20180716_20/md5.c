#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define READ_MAX_SIZE 1024
#define MD5_SIZE 16
#define MD5_STR_LEN (MD5_SIZE * 2)

int get_md5(char *path, char *md5_str)
{
	int fd;
	int ret = 0;
	int i = 0;
	unsigned char data[READ_MAX_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;
	printf("%s\n", path);
	if ((fd = open(path, O_RDWR, 0666)) < 0)
	{
		perror("open failed");
		return -1;
	}
	MD5_Init(&md5);
	//struct stat f_stat;
	//fstat(fd, &f_stat);
	//total_read = 0;
	while(1/*total_read < f_stat.st_size*/)
	{
		ret = read(fd, data, READ_MAX_SIZE);
		if (ret == -1)
		{
			perror("read failed");
			printf("%d\n", errno);
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

int main(int argc, char **argv)
{
	printf("%s\n", argv[1]);
	char buffer[32] = {0};
	get_md5(argv[1], buffer);
	printf("%s\n", buffer);
	return 0;
}

