#include "header.h"

//使用清理函数

void clean_fun(void *p)
{
	printf("I am cleanup fun.\n");
	free(p);
}

void *thread_fun(void *p)
{
	printf("child thread\n");
	p = malloc(20);
	pthread_cleanup_push(clean_fun, p);
	sleep(2);
	pthread_exit((void*)5);	//先调用p_exit函数，函数退出会自动弹出清理函数
	pthread_cleanup_pop(0);
}

int main()
{
	pthread_t pth_id;
	pthread_create(&pth_id, NULL, thread_fun, NULL);
	sleep(1);
	long retval;
	pthread_join(pth_id, (void**)&retval);
	printf("retval = %ld\n", retval);
	return 0;
}
