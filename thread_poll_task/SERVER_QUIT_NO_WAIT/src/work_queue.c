#include "work_queue.h"

int queue_init(pqueue_t q)
{
	q->size = 0;	//队列长度初始化为0
	q->head = (pnode_t)calloc(1, sizeof(node_t));
	q->tail = q->head;
	return 1;
}

int queue_insert(pqueue_t q, const pnode_t ptr)
{
#ifdef QUEUE_CAPACITY
	if(q->size == QUEUE_CAPACITY)	//队列已经满，则不插入
		return -1;
#endif
	if(0 == q->size)	//队列为空
	{
		q->head->next = ptr;
		q->tail = ptr;
	}
	else
	{
		q->tail->next = ptr;
		q->tail = ptr;
	}
	q->size++;
	return 1;
}

int queue_pop(pqueue_t q, pnode_t *buf)
{
	if(0 == q->size)	//队列为空
	{
		return -1;
	}
	*buf = q->head->next;
	if(0 == q->size--)
	{
		q->head->next = NULL;
		q->tail = q->head;
	}
	else
	{
		q->head->next = (*buf)->next;
	}
	return 1;
}

int queue_delete(pqueue_t q)
{
	pnode_t ptr;
	while(q->head->next)
	{
		ptr = q->head->next;
		q->head->next = ptr->next;
		free(ptr);
	}
	free(q->head);
	return 1;
}
