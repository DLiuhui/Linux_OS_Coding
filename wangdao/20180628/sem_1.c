#include "header.h"
//多个信号量

int main(int argc, char **argv)
{
	int sem_id;
	sem_id = semget(1000, 2, IPC_CREAT|0600);
	check_error(-1, sem_id, "semget");
	unsigned short arr[2] = {1, 5};
	int ret = semctl(sem_id, 0, SETALL, arr);
	check_error(-1, ret, "semctl");
	memset(arr, 0, sizeof(arr));	//将arr清零
	ret = semctl(sem_id, 0, GETALL, arr);	//获得信号值
	printf("sem1 = %d, sem2 = %d\n", arr[0], arr[1]);
	return 0;
}
