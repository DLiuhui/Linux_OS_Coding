#include "header.h"
#include <sys/ipc.h>
#include <sys/shm.h>
//6、新建共享内存，连接，然后往共享内存写入How are you,然后另外一个进程连接共享内存，读取数据并打印显示

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
	char *p;
	p = (char*)shmat(shm_id, NULL, 0);
	if((char*)-1 == p)
	{
		perror("shmat");
		shmctl(shm_id, IPC_RMID, NULL);	//删除共享空间
		return -1;
	}
	p[0] = '\0';	//对p初始化
	pid_t n_pid = fork();
	if(n_pid < 0){
		perror("fork");
		return -1;
	}else if(n_pid == 0){
		sprintf(p, "How are you?");
		return 1;
	}else{
		while(p[0] == '\0');
		printf("%s\n", p);
		shmctl(shm_id, IPC_RMID, NULL);	//删除共享空间
		return 0;
	}
}
