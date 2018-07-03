#include "header.h"

int main()
{
	unsigned short port = 0x1234;
	printf("%d\n", port);
	unsigned short nport = htons(port);	//小端转大端
	printf("%d\n", nport);
	return 0;
}
