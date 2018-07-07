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
	return NULL;
}

int main()
{
	pthread_t thread_id;
	data d;
	d.sum = 0;
	int ret;
	ret = pthread_mutex_init(&d.mutex, NULL);	//锁初始化
	check_thread_error(ret, "pthread_mutex_init");
	struct timeval start, end;
	gettimeofday(&start, NULL);
	ret = pthread_create(&thread_id, NULL, thread_func, &d);
	check_thread_error(ret, "pthread_create");
	for(int i = 0; i < N; i++)
	{
		pthread_mutex_lock(&d.mutex);
		d.sum = d.sum + 1;
		pthread_mutex_unlock(&d.mutex);
	}
	pthread_join(thread_id, NULL);
	gettimeofday(&end, NULL);
	printf("sum = %d\n", d.sum);
	printf("use time=%ld\n",(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec);
	return 0;
}
