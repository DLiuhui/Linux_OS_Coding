#include "header.h"

int main(int argc,char**argv)
{
	argc_check(argc,3);
	int fdw,fdr;
	fdw=open(argv[1],O_WRONLY);//只有一端会阻塞
	if(-1==fdw)
	{
		perror("open");
		return -1;
	}
	fdr=open(argv[2],O_RDONLY);
	//printf("fdw=%d,fdr=%d\n",fdw,fdr);
	char buf[128];
	fd_set rdset;
	int ret;
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(0,&rdset);	//0对应标准输入句柄
		FD_SET(fdr,&rdset);	//fdr对应可读管道句柄
		ret=select(fdr+1,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{
			if(FD_ISSET(fdr,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(fdr,buf,sizeof(buf));//对方结束通信，fdr会被设置，如果不加入退出机制会陷入死循环，由于缓冲区为空，所以read返回0，作为退出机制
				if(0==ret)
				{
					printf("对方已经下线\n");
					break;
				}
				printf("%s\n",buf);
			}
			//监视标准输入句柄，如果检测到输入，则将输入输出到管道文件句柄中
			if(FD_ISSET(0,&rdset))
			{
				memset(buf,0,sizeof(buf));
				read(0,buf,sizeof(buf));
				write(fdw,buf,strlen(buf)-1);
			}
		}
	}
	return 0;
}
