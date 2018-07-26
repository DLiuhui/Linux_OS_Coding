#include "header.h"

int main(int argc, char **argv)
{
	FILE *fp = fopen("file", "a");
	if(fp == NULL)
	{
		perror("fopen error");
		exit(-1);
	}
	fputs("world\n", fp);
	fclose(fp);
	return 0;
}
