#include "header.h"

int main(int argc,char**argv)
{
	int sem_id;
	sem_id=semget(2000,1,IPC_CREAT|0600);
	check_error(-1,sem_id,"semget");
	int ret;
	ret=semctl(sem_id,0,SETVAL,1);
	check_error(-1,ret,"semctl");
	int shmid;
	shmid=shmget(1000,4096,0600|IPC_CREAT);
	if(-1==shmid)
	{
		perror("shmget");
		return -1;
	}
	printf("shmid=%d\n",shmid);
	int *p;
	p=(int*)shmat(shmid,NULL,0);
	if((int*)-1==p)
	{
		perror("shmat");
		return -1;
	}
	p[0]=50;
	int i;
	struct sembuf sopp,sopv;
	sopp.sem_num=0;
	sopp.sem_op=-1;//加锁
	sopp.sem_flg=SEM_UNDO;
	sopv.sem_num=0;
	sopv.sem_op=1;//解锁
	sopv.sem_flg=SEM_UNDO;
	int fds[2];
	pipe(fds);
	if(!fork())
	{
		close(fds[0]);
		write(fds[1],&ret,sizeof(ret));	//子进程唤醒父进程
		while(1)
		{
			semop(sem_id,&sopp,1);
			if(p[0]>0)
			{
				printf("windows1 start sale tickets %d\n",p[0]);
				p[0]--;
				printf("windows1 sale finish tickets %d\n",p[0]);
				semop(sem_id,&sopv,1);
			}else{
				semop(sem_id,&sopv,1);
				break;
			}
		}
	}else{
		char buf[5];
		close(fds[1]);
		read(fds[0],buf,sizeof(buf));	//父进程等待被唤醒
		while(1)
		{
			semop(sem_id,&sopp,1);
			if(p[0]>0)
			{
				printf("windows2 start sale tickets %d\n",p[0]);
				p[0]--;
				printf("windows2 sale finish tickets %d\n",p[0]);
				semop(sem_id,&sopv,1);
			}else{
				semop(sem_id,&sopv,1);
				break;
			}
		}
		wait(NULL);
		printf("sale over tickets p[0]=%d\n",p[0]);
		shmctl(shmid, IPC_RMID, NULL);
		semctl(sem_id, 0, IPC_RMID);
	}
	return 0;
}
