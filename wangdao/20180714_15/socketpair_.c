#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define SOCKET_BUFFER_SIZE	1000

void *thread_func(void *arg)
{
	int len = 0;
	int fd = *((int*)arg);
	char buf[500];
	int cnt = 0;
	while(1)
	{
		bzero(buf, sizeof(buf));
		len = sprintf(buf, "Hi, main process, cnt = %d", cnt++);
		write(fd, buf, len);
		len = read(fd, buf, sizeof(buf));
		printf("%s\n", buf);
		sleep(2);
	}
	return NULL;
}

int main()
{
	int ret_num;
	int sockets[2];
	int buffer_size = SOCKET_BUFFER_SIZE;
	pthread_t thread;
	ret_num = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
	if(ret_num == -1)
	{
		printf("socketpair create error\n");
		return -1;
	}
	setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size));
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size));
	setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size));
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size));
	pthread_create(&thread, NULL, thread_func, (void*)(&sockets[1]));

	int len = 0;
	int fd = sockets[0];
	char buf[500];
	int cnt = 0;
	while(1)
	{
		bzero(buf, sizeof(buf));
		len = read(fd, buf, 500);
		printf("%s\n", buf);
		len = sprintf(buf, "Hi, thread process, cnt = %d", cnt++);
		write(fd, buf, len);
	}
	return 0;
}
