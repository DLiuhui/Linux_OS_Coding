#include "header.h"

int main()
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGINT);
	int ret;
	ret=sigprocmask(SIG_BLOCK,&mask,NULL);
	check_error(-1,ret,"sigprocmask");
	sleep(10);
	printf("you can see me\n");
	sigset_t pend;
	sigemptyset(&pend);
	sigpending(&pend);
	if(sigismember(&pend,SIGINT))
	{
		printf("SIGINT is pending\n");
	}else{
		printf("SIGINT is not pending\n");
	}
	ret=sigprocmask(SIG_UNBLOCK,&mask,NULL);
	check_error(-1,ret,"sigprocmask");
	return 0;
}
