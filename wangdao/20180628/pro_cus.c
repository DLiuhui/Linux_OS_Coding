#include "header.h"

//创建两个信号量，一个作为仓库量（0），一个作为消费者可以使用的生产量（1）
//当仓储量为0，无法生产，当生产量为0，无法消费
int main()
{
	int sem_id;
	sem_id=semget(1000,2,IPC_CREAT|0600);	//两个信号量
	check_error(-1,sem_id,"semget");
	unsigned short arr[2]={10,0};
	int ret=semctl(sem_id,0,SETALL,arr);
	check_error(-1,ret,"semctl");
	memset(arr,0,sizeof(arr));
	ret=semctl(sem_id,0,GETALL,arr);
	printf("store num=%d,product num=%d\n",arr[0],arr[1]);
	struct sembuf sopp,sopv;
	if(!fork())
	{
		//生产者
		sopp.sem_num=0;
		sopp.sem_op=-1;
		sopp.sem_flg=SEM_UNDO;
		sopv.sem_num=1;
		sopv.sem_op=1;
		sopv.sem_flg=SEM_UNDO;
		while(1)
		{
			printf("I am productor,store num=%d,product num=%d\n",semctl(sem_id,0,GETVAL),semctl(sem_id,1,GETVAL));
			semop(sem_id,&sopp,1);	//对0号信号量进行操作，检查仓库是否>0
			printf("producing...\n");
			semop(sem_id,&sopv,1);	//生产后，生产量+1，仓储量-1
			printf("I am productor,store num=%d,product num=%d\n",semctl(sem_id,0,GETVAL),semctl(sem_id,1,GETVAL));
			sleep(1);
		}
		exit(0);
	}else{
		//消费者
		sopp.sem_num=1;
		sopp.sem_op=-1;
		sopp.sem_flg=SEM_UNDO;
		sopv.sem_num=0;
		sopv.sem_op=1;
		sopv.sem_flg=SEM_UNDO;
		while(1)
		{
			printf("I am customer,store num=%d,product num=%d\n",semctl(sem_id,0,GETVAL),semctl(sem_id,1,GETVAL));
			semop(sem_id,&sopp,1);	//对1号信号量操作，检查生产量是否为>0
			printf("consuming...\n");
			semop(sem_id,&sopv,1);	//消费后，仓库量+1，生产量-1
			printf("I am customer,store num=%d,product num=%d\n",semctl(sem_id,0,GETVAL),semctl(sem_id,1,GETVAL));
			sleep(2);
		}
		wait(NULL);
	}
	return 0;
}
