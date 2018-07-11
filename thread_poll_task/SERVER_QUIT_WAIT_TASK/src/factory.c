#include "factory.h"

//主线程初始化
int factory_init(pfactory_t fac, thread_func_t threadfunc, int th_num)
{
	bzero(fac, sizeof(factory_t));
	fac->th_id = (pthread_t*)calloc(th_num, sizeof(pthread_t));
	fac->th_num = th_num;
	fac->task_q = (pqueue_t)calloc(1, sizeof(queue_t));
	queue_init(fac->task_q);
	pthread_cond_init(&fac->cond, NULL);
	pthread_mutex_init(&fac->mutex, NULL);
	fac->threadfunc = threadfunc;
	fac->work_flag = 0;
	return 0;
}

//启动工程，创建线程
int factory_start(pfactory_t pf)
{
	if(!pf->work_flag)	//工程未启动
	{
		int i;
		for(i = 0; i < pf->th_num; i++)
		{
			pthread_create(pf->th_id + i, NULL, pf->threadfunc, pf);
		}
		pf->work_flag = 1;
	}
	return 0;
}
