#include <stdio.h>
#include "header.h"
#define FILENAME "stuData"
#define ROOT 0664
typedef struct student
{
	char ID[10];
	char name[10];
	float score;
}student;

int main(int argc, char **argv)
{
	argc_check(argc, 2);
	student stu[3] = {
		{"2001", "ddrh", 90.0},
		{"2002", "rrrr", 91.0},
		{"2003", "zzz", 92.0}
	};
	int fp = open(argv[1], O_RDWR|O_CREAT, ROOT);
	int ret = write(fp, stu, sizeof(stu));
	if(ret == -1)
	{
		perror("write");
		return -1;
	}
	off_t offset = lseek(fp, 0, SEEK_SET);
	if(offset == -1)
	{
		perror("lseek");
		return -1;
	}
	student *stup = (student*)malloc(sizeof(student));
	//使用read之前一定要为stup申请空间，不然会读取失败，ret=-1
	while((ret = read(fp, stup, sizeof(student))) != 0)
	{
		if(ret != -1)
		{
			printf("%s %s %.2f\n", stup->ID, stup->name, stup->score);
		}
		else
		{
			perror("read");
			free(stup);
			return -1;
		}
	}
	free(stup);
	return 0;
}
