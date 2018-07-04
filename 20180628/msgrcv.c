#include "header.h"
//消息队列收消息
struct msgbuf{
	long mtype;
	char buf[64];
};
int main()
{
	int msg_id=msgget(1000,IPC_CREAT|0600);
	check_error(-1,msg_id,"msgget");
	struct msgbuf msg;
	memset(&msg,0,sizeof(msg));
	msg.mtype=0;//一定要是正整数
	int ret=msgrcv(msg_id,&msg,sizeof(msg.buf),2,0);	//先接收发送时，类型为2的消息
	check_error(-1,ret,"msgrcv");
	printf("msg=%s\n",msg.buf);
	msgctl(msg_id, IPC_RMID, NULL);
	return 0;
}
