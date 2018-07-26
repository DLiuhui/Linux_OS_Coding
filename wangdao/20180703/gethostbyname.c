#include "header.h"
//得到网站域名等

int main(int argc,char* argv[])
{
	check_argc(argc,2);
	struct hostent *p;
	p=gethostbyname(argv[1]);
	if(NULL==p)
	{
		printf("gethostbyname failed\n");
		return -1;
	}
	printf("h_name=%s\n",p->h_name);
	int i;
	for(i=0;p->h_aliases[i]!=NULL;i++)
	{
		puts(p->h_aliases[i]);
	}
	printf("h_length=%d\n",p->h_length);
	char ip[16]={0};
	for(i=0;p->h_addr_list[i]!=NULL;i++)
	{
		memset(ip,0,sizeof(ip));
		inet_ntop(p->h_addrtype,p->h_addr_list[i],ip,sizeof(ip));
		puts(ip);
	}
}
