#include "header.h"
#define SALE_WINDOWS 4	//抢票窗口数目
#define SET_WINDOWS 1	//放票窗口数目
#define TOTAL_WINDOWS SALE_WINDOWS+SET_WINDOWS	//窗口总数
#define TIMES 3	//放票次数

typedef struct{
	int ticket_num;
	pthread_cond_t cond;	//条件变量
	pthread_mutex_t mutex;	//锁
	int windows;	//抢票窗口
	int fds[2];	//读写句柄
	int trigger;	//触发器
	Boolean thread_ready;
}data;

//抢票窗口
void *saleWindows(void *p)
{
	data *d = (data*)p;
	int windows = d->windows;
	d->thread_ready = TRUE;
	//write(d->fds[1], &windows, sizeof(windows));
	pthread_mutex_lock(&d->mutex);	//加锁
	d->trigger++;	//递增开关
	pthread_mutex_unlock(&d->mutex);	//解锁
	int total_sale = 0;	//统计售票总数
	while(1)
	{
		pthread_mutex_lock(&d->mutex);	//加锁
		if(d->ticket_num > 0)
		{
			d->ticket_num--;
			printf("windows %d sale a ticket\nThere remains %d tickets\n", windows, d->ticket_num);
			total_sale++;
			if(d->ticket_num == 0)
			{
				pthread_cond_signal(&d->cond);//信号触发，放票
			}
		}else{
			pthread_mutex_unlock(&d->mutex);
			printf("windows %d total sale = %d\n", windows, total_sale);
			break;
		}
		pthread_mutex_unlock(&d->mutex);	//解锁
		usleep(10000);
	}
}

void *setWindows(void *p)
{
	data *d = (data*)p;
	int windows = d->windows;
	int times = 0;
	while(times++ < TIMES)
	{
		pthread_mutex_lock(&d->mutex);	//加锁
		if(d->ticket_num > 0)
		{
			pthread_cond_wait(&d->cond, &d->mutex);	//等待放票信号
		}
		d->ticket_num = 20;
		printf("set windows %d: put %d more tickets\n", windows, d->ticket_num);
		pthread_mutex_unlock(&d->mutex);	//解锁
	}
}

int main(int argc, char **argv)
{	
	pthread_t pth[TOTAL_WINDOWS];	//总线程数目
	data t;	//定义全局变量
	pipe(t.fds);	//使用管道进行线程同步
	t.ticket_num = 20;
	pthread_mutex_init(&t.mutex, NULL);
	pthread_cond_init(&t.cond, NULL);
	t.trigger = 0;
	t.thread_ready = FALSE;
	int i;	//计数
	char flag;
	for(i = 0; i < SALE_WINDOWS; i++)
	{
		t.windows = i + 1;
		pthread_create(pth+i, NULL, saleWindows, &t);
		while(FALSE == t.thread_ready);
		t.thread_ready = FALSE;
		//read(t.fds[0], &flag, sizeof(flag));	//当子线程触发写管道操作，主线程开启下一子线程，让子线程有序开始
	}
	while(t.trigger != SALE_WINDOWS);	//等待开启放票窗口
	for(i = 0; i < SET_WINDOWS; i++)
	{
		t.windows = i + 1;
		pthread_create(pth+SALE_WINDOWS+i, NULL, setWindows, &t);
	}
	for(i = 0; i < TOTAL_WINDOWS; i++)
	{
		pthread_join(pth[i], NULL);	//无需返回值
	}
	return 0;
}
