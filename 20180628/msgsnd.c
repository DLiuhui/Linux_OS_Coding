#include "header.h"
//消息队列发消息
struct msgbuf{
	long mtype;
	char buf[64];
};
int main(int argc,char**argv)
{
	check_argc(argc,3);
	int msg_id=msgget(1000,IPC_CREAT|0600);
	check_error(-1,msg_id,"msgget");
	struct msgbuf msg;
	msg.mtype=atoi(argv[1]);//一定要是正整数，决定消息的类型
	strcpy(msg.buf,argv[2]);
	int ret=msgsnd(msg_id,&msg,strlen(msg.buf),0);
	check_error(-1,ret,"msgsnd");
	return 0;
}
