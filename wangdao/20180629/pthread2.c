#include "header.h"
//创建线程
//合并线程

//线程函数
void *thread_func(void *p)
{
	printf("I am child thread\n");
	p = malloc(20 * sizeof(char));
	strcpy((char*)p, "hello");
	sleep(3);
	pthread_exit(p);
}

int main(int argc, char **argv)
{
	pthread_t thread_id;
	int ret;
	ret = pthread_create(&thread_id, NULL, thread_func, NULL);
	check_thread_error(ret, "pthread_creat");
	printf("I am main thread\n");
	void *thread_ret;
	ret = pthread_join(thread_id, &thread_ret);	//类似于进程里的wait，等待子线程结束
	check_thread_error(ret, "pthread_join");
	printf("main thread %s\n", (char*)thread_ret);
	return 0;
}
