#include "header.h"

//os concepts 7th
//P87
//共享空间通信的 生产者与消费者模型
//具有亲缘关系的生产消费模型
//共享缓冲，通过 循环数组/循环队列实现
//使用信号量机制进行加 解锁，保证操作的原子性

#define BUFFER_SIZE 10	//共享空间缓冲限制
#define SEM_MODE 0666
#define RUNNING_TIME 10	//控制运行时间

typedef int item;	//流通项目

union sem_un {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main()
{
	int sem_id = semget(IPC_PRIVATE, 1, SEM_MODE|IPC_CREAT);	//创建信号量集
	check_error(-1, sem_id, "semget");
	pid_t pid = fork();
	check_error(-1, pid, "fork");
	if(pid == 0){	//生产者进程
		int t_child = 0;
		struct sembuf sem;
		memset(&sem, 0, sizeof(struct sembuf));
		sem.sem_num = 0;	//信号量编号，第一个为0
		sem.sem_op = 1;	//+1表示生产操作
		sem.sem_flg = SEM_UNDO;
		union sem_un arg;
		arg.val = 0;	//初始化
		semctl(sem_id, 0, SETALL, arg);	//将数据全部设置到信号量集里面去，相当于公共数据
		while(1){
			semop(sem_id, &sem, 1);	//指定执行V操作，生产操作
			printf("productor total number:%d\n", semctl(sem_id, 0, GETVAL));	//获得公共值
			if(t_child++ == RUNNING_TIME)
				break;
			sleep(1);	//生产间隙
		}
		exit(EXIT_SUCCESS);
	}else{	//消费者进程
		sleep(2);	//先让子进程生产
		int t_pare = 0;
		struct sembuf sem;
		memset(&sem, 0, sizeof(struct sembuf));
		sem.sem_num = 0;
		sem.sem_op = -1;	//消费
		sem.sem_flg = SEM_UNDO;
		while(1){
			semop(sem_id, &sem, 1);
			printf("costomer total number:%d\n", semctl(sem_id, 0, GETVAL));
			if(t_pare++ == RUNNING_TIME)
				break;
			sleep(2);
		}
		semctl(sem_id, 0, IPC_RMID);	//删除共享空间
		exit(EXIT_SUCCESS);
	}
}
