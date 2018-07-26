#include "header.h"
//IPC_PRIVATE方式新建消息队列，fork子进程，父进程发送消息，子进程读取并打印，父进程回收子进程后，删除消息队列
struct msgbuf{
	long mtype;
	char buf[100];
};
int main(int argc, char **argv)
{
	int mq_id = msgget(IPC_PRIVATE, 0600|IPC_CREAT);
	check_error(-1, mq_id, "msgget");
	pid_t pid = fork();
	if(pid == 0){
		struct msgbuf rece;
		msgrcv(mq_id, &rece, sizeof(rece.buf), 1, 0);
		printf("child get: %s\n", rece.buf);
		exit(EXIT_SUCCESS);
	}else{	//父进程发送信息
		struct msgbuf mess;
		mess.mtype = 1;
		strcpy(mess.buf, "good to do");
		printf("parent sending...\n");
		int ret = msgsnd(mq_id, &mess, strlen(mess.buf), 0);
		check_error(-1, ret, "msgsnd");
		wait(NULL);
		msgctl(mq_id, IPC_RMID, NULL);
		exit(EXIT_SUCCESS);
	}
}
