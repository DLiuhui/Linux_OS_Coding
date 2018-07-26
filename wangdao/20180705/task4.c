#include "header.h"

void *thread_func(void *p)
{
	sleep(2);
	time_t t = time(NULL);
	char *p1;
	p1 = ctime(&t);
	printf("child: p1=%p, time=%s\n", p1, p1);
}

int main()
{
	pthread_t c_pthr_id;
	pthread_create(&c_pthr_id, NULL, thread_func, NULL);
	char *p;
	time_t t = time(NULL);
	p = ctime(&t);
	printf("main: p=%p, time=%s\n", p, p);
	pthread_join(c_pthr_id, NULL);
	sleep(4);
	printf("main: p=%p, time=%s\n", p, p);
}
