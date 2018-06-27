#include "header.h"
#include <sys/sem.h>
#define N 10000000
int main(int argc, char **argv)
{
	int shm_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT|0600);
	check_error(-1, shm_id, "shmget");
	int *p = (int*)shmat(shm_id, NULL, 0);
	p[0] = 0;
	int i;
	//创建信号
	int sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT|0600);	//申请信号量
	check_error(-1, sem_id, "semget");
	int ret = semctl(sem_id, 0, SETVAL, 1);	//要对信号量进行初始化！
	check_error(-1, ret, "semctl");
	struct sembuf sem_p = {0, -1, SEM_UNDO},	//加锁
				  sem_v = {0, 1, SEM_UNDO};	//解锁
	pid_t n_pid = fork();
	check_error(-1, n_pid, "fork");
	if(n_pid == 0){
		for(i = 0; i < N; i++){
			semop(sem_id, &sem_p, 1);
			p[0] = p[0] + 1;
			semop(sem_id, &sem_v, 1);
		}
		return 1;
	}else{
		for(i = 0; i < N; i++){
			semop(sem_id, &sem_p, 1);
			p[0] = p[0] + 1;
			semop(sem_id, &sem_v, 1);
		}
		wait(NULL);
		printf("Ok, p[0] = %d\n", p[0]);
	}
	shmctl(shm_id, IPC_RMID, NULL);
	//shmdt(p);	//断开链接
	semctl(sem_id, 0 , IPC_RMID);
	return 0;
}
