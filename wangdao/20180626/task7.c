#include "header.h"
#include <sys/ipc.h>
#include <sys/shm.h>

//7、两个进程对一个共享内存各加1000万，查看最后结果
#define N 10000000
int main(int argc, char **argv)
{
	argc_check(argc,2);
	key_t key = ftok(argv[1], 1);
	if(key == -1){	//key创建失败
		perror("ftok");
		return -1;
	}
	printf("key = %d\n", key);
	int shm_id;
	if((shm_id = shmget(key, 4096, 0666|IPC_CREAT|IPC_EXCL)) == -1){
	perror("shmget");
	return -1;
	}
	printf("shm_id=%d\n", shm_id);
	int *p;
	p = (int*)shmat(shm_id, NULL, 0);
	if((int*)-1 == p)
	{
		perror("shmat");
		shmctl(shm_id, IPC_RMID, NULL);	//删除共享空间
		return -1;
	}
	p[0] = 0;	//对p初始化
	int i;
	pid_t n_pid = fork();
	if(n_pid < 0){
		perror("fork");
		return -1;
	}else if(n_pid == 0){
		for(i = 0; i < N; i++)
			p[0]++;
		return 1;
	}else{
		for(i = 0; i < N; i++)
			p[0]++;
		wait(NULL);
		printf("p[0]=%d\n", p[0]);
		shmctl(shm_id, IPC_RMID, NULL);	//删除共享空间
		return 0;
	}
}
