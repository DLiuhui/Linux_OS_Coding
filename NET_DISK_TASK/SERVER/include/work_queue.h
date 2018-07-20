#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__
#include "header.h"

#define QUEUE_CAPACITY 1000	//利用宏开关定义队列最大量
//工作队列结点
typedef struct node_t
{
	int fd;	//客户端文件描述符
	int work_id;	//工作id
	char option[512];	//不同工作对应的选项
	struct node_t *next;
}node_t, *pnode_t;

typedef struct
{
	pnode_t head, tail;	//队列头尾指针
	int size;	//队列当前大小
	pthread_mutex_t queue_lock;	//队列锁
	pthread_cond_t queue_not_empty;	//非空信号
}queue_t, *pqueue_t;

int queue_init(pqueue_t);	//初始化队列
int queue_insert(pqueue_t, const pnode_t);	//插入结点
int queue_pop(pqueue_t, pnode_t*);	//节点出队
int queue_head_insert_exit(pqueue_t);	//退出时，在头部插入exit标记
int queue_delete(pqueue_t);	//删除队列
#endif
