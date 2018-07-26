#include "header.h"

int main(int argc, char **argv)
{
	system("ipcs");	//在写完后读，此时连接数为0
	key_t key = ftok("file",1); //1. 读入端用 ftok 函数获得 key
	if(key == -1)
	{
		perror("ftok");
		return -1;
	}
	int shm_id = shmget(key, 4096, 0600|IPC_CREAT); //2. 读入端用 shmget 函数打开共享内存段
		if(shm_id == -1)
		{
			perror("shmget");
			exit(-1);
		}
	char *pMap = (char*)shmat(shm_id, NULL, 0); //3. 获得共享内存段的首地址
	printf("receive the data:%s\n",pMap); //4. 读取共享内存段中的内容
	if(shmctl(shm_id, IPC_RMID, NULL) == -1) //5. 删除共享内存段
	{
		perror("shmctl");
		exit(-1);
	}
	system("ipcs");	//链接数为1，共享空间仍然存在，因为本进程尚未断开链接，shmctl只是标记删除，连接数不为0，并不能删除
	if(shmdt(pMap) == -1){
		perror("shmdt");
		exit(-1);
	}
	system("ipcs");	//会发现共享空间已经删除
	return 0;
}
