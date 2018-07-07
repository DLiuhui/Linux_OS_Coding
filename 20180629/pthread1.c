#include "header.h"
//创建线程

//线程函数
void *thread_func(void *p)
{
	printf("I am child thread\n");
}

int main(int argc, char **argv)
{
	pthread_t thread_id;
	int ret;
	ret = pthread_create(&thread_id, NULL, thread_func, NULL);
	check_thread_error(ret, "pthread_creat");
	printf("I am main\n");
	sleep(1);
	return 0;
}
