#include "header.h"

typedef struct {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
}data_type;

void *thread_func(void *p)
{
	data_type *data = (data_type*)p;
	printf("before wait cond\n");
	pthread_mutex_lock(&data->mutex);
	pthread_cond_wait(&data->cond, &data->mutex);
	pthread_mutex_unlock(&data->mutex);
	printf("I am child, I will exit\n");
}

int main()
{
	data_type data;
	pthread_mutex_init(&data.mutex, NULL);
	pthread_cond_init(&data.cond, NULL);
	pthread_t pth_id;
	pthread_create(&pth_id, NULL, thread_func, &data);
	printf("sleep\n");
	sleep(1);
	pthread_cond_signal(&data.cond);	//发送信号
	pthread_join(pth_id, NULL);
	pthread_mutex_destroy(&data.mutex);
	pthread_cond_destroy(&data.cond);
	printf("I am main thread\n");
	return 0;
}
