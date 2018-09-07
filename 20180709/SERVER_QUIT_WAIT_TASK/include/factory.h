#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "header.h"
#include "work_queue.h"
#include "TCP_link.h"
#include "tran_file.h"

typedef void* (*thread_func_t)(void*);
typedef struct
{
	pthread_t *th_id;	//线程id的起始地址
	int th_num;	//存储线程数目
	pqueue_t task_q;	//任务队列
	thread_func_t threadfunc;	//线程函数
	pthread_cond_t cond;	//信号
	pthread_mutex_t mutex;	//锁
	short work_flag;	//线程是否工作标志，可以用于退出判断
}factory_t, *pfactory_t;

int factory_init(pfactory_t, thread_func_t, int);
int factory_start(pfactory_t);


#endif
