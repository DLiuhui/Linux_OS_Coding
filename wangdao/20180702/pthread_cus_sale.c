#include "header.h"
//利用线程实现买家与卖家模型

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int store;	//仓储量
	int product;	//产品数目
	Boolean pthread_ready;
	struct timespec wait_t;
}data_type;

void *producer_fun(void *p)
{
	data_type *data = (data_type*)p;
	data->pthread_ready = TRUE;
	int total_pro = 0;
	while(1)
	{
		pthread_mutex_lock(&data->mutex);
		if(data->store > 0)
		{
			data->store--;
			data->product++;
			total_pro++;
			pthread_mutex_unlock(&data->mutex);
			printf("product %d, store %d, goods %d\n", 
					total_pro, data->store, data->product);
		}else{
			pthread_mutex_unlock(&data->mutex);
			printf("store empty\n");
			break;
		}
		usleep(800);
	}
}

void *customer_fun(void *p)
{
	data_type *data = (data_type*)p;
	int total_sale = 0;
	time_t start, end;
	while(1)
	{
		pthread_mutex_lock(&data->mutex);
		if(data->product > 0)
		{
			data->product--;
			total_sale++;
			printf("purchase %d, goods %d\n", total_sale, data->product);
			pthread_mutex_unlock(&data->mutex);
		}else{
			//data->wait_t.tv_sec = time(NULL) + 3;
			pthread_mutex_unlock(&data->mutex);
			start = time(NULL);
			while((end = time(NULL)) - start < 3 && data->product == 0);
			if(data->product == 0)
			{
				printf("There is no more goods. Saling over!\n");
				break;
			}
			//pthread_cond_timedwait(&data->cond, &data->mutex, &data->wait_t);
			//pthread_mutex_unlock(&data->mutex);
			//end = time(NULL);
			//printf("end %ld, start %ld\n", start, end);
			//if(end - start >= 2)
				//break;
		}
		usleep(500);
	}
}
int main()
{
	data_type data;
	pthread_t pth_id[2];
	pthread_mutex_init(&data.mutex, NULL);
	pthread_cond_init(&data.cond, NULL);
	data.store = 20;
	data.product = 10;
	data.pthread_ready = FALSE;
	pthread_create(&pth_id[0], NULL, producer_fun, &data);
	while(FALSE == data.pthread_ready);	//保证生产者在消费者之前行动
	pthread_create(&pth_id[1], NULL, customer_fun, &data);
	int i;
	for(i = 0; i < 2; i++)
	{
		pthread_join(pth_id[i], NULL);
	}
	return 0;
}
