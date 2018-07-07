#include "header.h"

//子进程控制
void child_handle(int fds)
{
	int new_fd;
	char notbusy_flag = 1;
	while(1)
	{
		recv_fd(fds, &new_fd);
		tran_file(new_fd);
		//printf("OK\n");
		//sleep(10);
		write(fds, &notbusy_flag, 1);
	}
}

//子进程接收文件句柄
void recv_fd(int sfd, int *fd)
{
	struct msghdr msg;
	bzero(&msg, sizeof(msg));	//如果不进行清空，子进程完成文件传输后可能会出现接收问题
	char buf[10]="hello";
	char buf1[10]="world";
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
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	//printf("pid = %d\n", getpid());	//所有的子进程将会在recvmsg处等待主进程发送
	int ret;
	ret = recvmsg(sfd, &msg, 0);
	//printf("get pid = %d\n", getpid());
	check_error(-1, ret, "recvmsg");
	*fd = *(int*)CMSG_DATA(cmsg);
}

int tran_file(int new_fd)
{
	Train t;
	//先发送文件名
	t.len = strlen(FILENAME);
	strcpy(t.buf, FILENAME);
	send_n(new_fd, (char*)&t, 4+t.len);
	//发送文件大小
	struct stat f_stat;
	stat(FILENAME, &f_stat);
	t.len = sizeof(f_stat.st_size);
	memcpy(t.buf, &f_stat.st_size, sizeof(off_t));
	send_n(new_fd, (char*)&t, 4+t.len);
	//发文件内容
	int fd = open(FILENAME, O_RDONLY);
	check_error(-1, fd, "open");
	while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
	{
		send_n(new_fd, (char*)&t, 4+t.len);
	}
	send_n(new_fd, (char*)&t, 4+t.len);
	return 0;
}
