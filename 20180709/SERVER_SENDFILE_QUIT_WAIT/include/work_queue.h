#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__
#include "header.h"

//#define QUEUE_CAPACITY 100	//利用宏开关定义队列最大量
//工作队列结点
typedef int data_t;
typedef struct node_t
{
	data_t data;	//队列数据
	struct node_t *next;
}node_t, *pnode_t;

typedef struct
{
	pnode_t head, tail;	//队列头尾指针
	int size;	//队列当前大小
}queue_t, *pqueue_t;

int queue_init(pqueue_t);	//初始化队列
int queue_insert(pqueue_t, const pnode_t);	//插入结点
int queue_pop(pqueue_t, pnode_t*);	//节点出队
int queue_delete(pqueue_t);	//删除队列
#endif
