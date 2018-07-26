#include "header.h"
//父进程工作
//包含父进程所需要进行的各种任务、函数

int make_child_process(pDataType p_data, int process_num)
{
	int i;	//循环计数
	int fds[2];	//管道两端
	pid_t pid;
	for(i = 0; i < process_num; i++)
	{
		socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);	//创建socket管道
		//创建子进程
		pid = fork();
		if(0 == pid)
		{
			close(fds[1]);
			child_handle(fds[0]);	//子进程在child_handle进行退出
		}
		close(fds[0]);
		//父进程记录子进程信息
		p_data[i].pid = pid;
		p_data[i].fds = fds[1];	//子进程管道对端
		p_data[i].busy_flag = FALSE;
	}
	return 0;
}

//发送文件句柄
void send_fd(int sfd, int fd)
{
	struct msghdr msg;
	bzero(&msg, sizeof(msg));
	char buf[10]="pare";
	char buf1[10]="fork";
	struct iovec iov[2];
	iov[0].iov_base=buf;
	iov[0].iov_len=strlen(buf);
	iov[1].iov_base=buf1;
	iov[1].iov_len=strlen(buf1);
	msg.msg_iov = iov;
	msg.msg_iovlen = sizeof(iov)/sizeof(struct iovec);
	struct cmsghdr *cmsg;
	int len = CMSG_LEN(sizeof(int));
	cmsg = (struct cmsghdr*)calloc(1, len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg) = fd;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	int ret;
	ret = sendmsg(sfd, &msg, 0);
	check_error(-1, ret, "sendmsg");
}

