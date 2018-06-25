#include "header.h"

int main(int argc,char**argv)
{
	argc_check(argc,3);
	int fdr,fdw;
	fdr=open(argv[1],O_RDONLY);
	if(-1==fdr)
	{
		perror("open1");
		return -1;
	}
	fdw=open(argv[2],O_WRONLY);
	//printf("fdr=%d,fdw=%d\n",fdr,fdw);
	char buf[128]={0};
	int ret;
	fd_set rdset;
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(0,&rdset);
		FD_SET(fdr,&rdset);
		ret=select(fdr+1,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{
			if(FD_ISSET(fdr,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(fdr,buf,sizeof(buf));//缓冲区为空的时候，读端会卡主
				if(0==ret)
				{
					printf("对方已经下线\n");
					break;
				}
				printf("%s\n",buf);
			}
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
