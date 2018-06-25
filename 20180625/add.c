#include "header.h"

int main(int argc , char **argv)
{
	argc_check(argc, 3);
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	printf("%d + %d = %d\n" , a , b , a + b);
	return 0;
}
