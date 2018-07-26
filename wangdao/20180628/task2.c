#include "header.h"
//2、signal设置信号处理行为，演示不同信号到来时，后来的信号会打断原有信号的信号处理函数效果，相同信号无法打断的效果

void SignFun(int signNo)
{
	if(signNo == SIGINT){
		printf("%d is coming\n", signNo);
		sleep(3);
		printf("%d has gone.\n", signNo);
	}else if(signNo == SIGQUIT){
		printf("This is %d.\n", signNo);
		sleep(2);
		printf("%d is over.\n", signNo);
	}
}
int main()
{
	signal(SIGINT, SignFun);
	signal(SIGQUIT, SignFun);
	while(1);
	return 0;
}
