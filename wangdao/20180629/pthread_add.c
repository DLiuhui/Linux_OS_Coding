#include "header.h"
#define N 10000000

typedef struct{
	int sum;
	pthread_mutex_t mutex;
}data;

void *thread_func(void *p)
{
	data *d = (data*)p;
	for(int i = 0; i < N; i++)
	{
		pthread_mutex_lock(&d->mutex);
		d->sum  = d->sum + 1;
		pthread_mutex_unlock(&d->mutex);
	}
	pthread_exit(NULL);
}

int main()
{
	pthread_t thread_id;
	data d;
	d.sum = 0;
	int ret;
	ret = pthread_mutex_init(&d.mutex, NULL);	//锁初始化
	check_thread_error(ret, "pthread_mutex_init");
	ret = pthread_create(&thread_id, NULL, thread_func, &d);
	check_thread_error(ret, "pthread_create");
	for(int i = 0; i < N; i++)
	{
		pthread_mutex_lock(&d.mutex);
		d.sum = d.sum + 1;
		pthread_mutex_unlock(&d.mutex);
	}
	pthread_join(thread_id, NULL);
	printf("sum = %d\n", d.sum);
	return 0;
}
