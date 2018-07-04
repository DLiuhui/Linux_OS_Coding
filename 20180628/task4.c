#include "header.h"
//4、sigaction实现2号信号，屏蔽3号信号的执行效果，同时在2号信号的信号处理流程中，判断3号信号是否被挂起
void sigFun(int sigNum, siginfo_t *p, void *ptr)
{
	printf("%d is coming.\n", sigNum);
	sleep(3);
	sigset_t pend;
	sigpending(&pend);
	if(sigismember(&pend, SIGQUIT)){
		printf("SIGQUIT is pending\n");
	}else{
		printf("SIGQUIT is not pending\n");
	}
	printf("%d is over.\n", sigNum);
}

int main()
{
	struct sigaction signAct;
	bzero(&signAct, sizeof(signAct));
	signAct.sa_sigaction = sigFun;
	signAct.sa_flags = SA_SIGINFO;
	sigemptyset(&signAct.sa_mask);
	sigaddset(&signAct.sa_mask, SIGQUIT);
	int ret = sigaction(SIGINT, &signAct, NULL);
	check_error(-1, ret, "sigaction");
	while(1);
}
