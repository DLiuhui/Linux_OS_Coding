#include "header.h"

int main(int argc, char **argv)
{
	key_t key = ftok("file", 1);
	if(key == -1){
		perror("ftok");
		return -1;
	}
	int shm_id = shmget(key, 4096, 0600|IPC_CREAT);
	if(shm_id == -1){
		perror("shmget");
		return -1;
	}
	char *pMap = (char*)shmat(shm_id, NULL, 0);	//自己申请指针，指针获得空间，有点像malloc
	memset(pMap, '\0', 4096);	//内存初始化
	strcpy(pMap, "hello world");	//写入
	system("ipcs");	//查看共享内存，根据实际效果，此时新创建的共享内存的连接数为1
	if(shmdt(pMap) == -1)	//断开与共享内存的链接
	{
		perror("shmdt");
		return -1;
	}
	system("ipcs");	//再次查看，根据实际效果，此时共享内存连接数为0
	return 0;
}
