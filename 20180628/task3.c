#include "header.h"
//3、sigaction实现2号，3号信号不断重入的执行效果
void sigFun(int sigNum, siginfo_t *p, void *ptr)
{
	if(sigNum == SIGINT){
		printf("%d:**\n", sigNum);
		sleep(2);
		printf("%d:**-\n", sigNum);
	}else if(sigNum == SIGQUIT){
		printf("%d:***\n", sigNum);
		sleep(2);
		printf("%d:***-\n", sigNum);
	}
}

int main()
{
	struct sigaction signAct;
	bzero(&signAct, sizeof(signAct));
	signAct.sa_sigaction = sigFun;
	signAct.sa_flags = SA_SIGINFO|SA_NODEFER;
	int ret = sigaction(SIGINT, &signAct, NULL);
	check_error(-1, ret, "sigaction");
	ret = sigaction(SIGQUIT, &signAct, NULL);
	check_error(-1, ret, "sigaction");
	while(1);
}
