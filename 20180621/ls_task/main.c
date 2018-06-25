#include "header.h"
#include "ls.h"

int main(int argc, char **argv)
{
	argc_check(argc, 2);
	ls(argv[1]);
	return 0;
}