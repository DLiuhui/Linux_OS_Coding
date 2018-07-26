#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define BUFFER_SIZE 20
#define OVER -1
typedef struct prodcons
{
	int buffer[BUFFER_SIZE];
	pthread_mutex_t mutex;
	int readpos, writepos;
	pthread_cond_t not_empty;	//不空，可以读
	pthread_cond_t not_full;	//不满，可以写
}prodcons;

void init(prodcons *p)
{
	pthread_mutex_init(&p->mutex, NULL);
	pthread_cond_init(&p->not_full, NULL);
	pthread_cond_init(&p->not_empty, NULL);
	p->readpos = 0;
	p->writepos = 0;
}

void destroy(prodcons *p)
{
	pthread_mutex_destroy(&p->mutex);
	pthread_cond_destroy(&p->not_full);
	pthread_cond_destroy(&p->not_empty);
}

void put(prodcons *p, int data)	//生产者
{
	pthread_mutex_lock(&p->mutex);	//先加锁再判断为空
	while((p->writepos + 1)%BUFFER_SIZE == p->readpos)	//队列判断满队？注意此处需要使用while在信号解除后再次询问（考虑到其它生产者可能先进行了生产使得buffer满）
	{
		printf("wait for not full\n");
		pthread_cond_wait(&p->not_full, &p->mutex);
	}
	//如果还没满
	p->buffer[p->writepos] = data;
	p->writepos++;
	if(p->writepos >= BUFFER_SIZE)
		p->writepos = 0;
	pthread_cond_signal(&p->not_empty);	//队列非空，可以消费
	pthread_mutex_unlock(&p->mutex);
}

int get(prodcons *p)	//消费者
{
	pthread_mutex_lock(&p->mutex);
	while(p->writepos == p->readpos)
	{
		printf("wait for not empty\n");
		pthread_cond_wait(&p->not_empty, &p->mutex);
	}
	//如果非空
	int data = p->buffer[p->readpos];
	p->readpos++;
	if(p->readpos >= BUFFER_SIZE)
		p->readpos = 0;
	pthread_cond_signal(&p->not_full);	//队列非满，可以生产
	pthread_mutex_unlock(&p->mutex);
	return data;
}

void *producer(void *data)
{
	prodcons *buffer = (prodcons*)data;
	int n = 0;
	for(; n < 50; n++)
	{
		printf("put-->%d\n", n);
		put(buffer, n);
	}
	put(buffer, OVER);
	printf("producer stopped\n");
	pthread_exit(NULL);
}

void *consumer(void *data)
{
	prodcons *buffer = (prodcons*)data;
	while(1)
	{
		int d = get(buffer);
		if(d == OVER)
			break;
		printf("%d-->get\n", d);
	}
	printf("consumer stopped\n");
	pthread_exit(NULL);
}

int main()
{
	pthread_t pro, con;
	prodcons buffer;
	init(&buffer);
	pthread_create(&pro, NULL, producer, &buffer);
	pthread_create(&pro, NULL, consumer, &buffer);
	pthread_join(pro, NULL);
	pthread_join(con, NULL);
	destroy(&buffer);
	return 0;
}
