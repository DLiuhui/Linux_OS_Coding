//采用深度优先搜索查找文件

#include "header.h"

void printdir(char *path, int width)
{
	DIR *dir;
	dir = opendir(path);
	if(!dir)
	{
		perror("opendir error");
		return;
	}
	struct dirent *p;	//文件、目录指针
	char buff[512] = {0};	//存储路径
	while((p = readdir(dir)) != NULL)
	{
		if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))	//此处，文件指针不能进入 . .. 文件夹，不然会一直在当前文件(.)或者退出当前目录(..)返回上一级目录。
			;
		else
		{
			printf("%*s%s\n", width, " ", p->d_name);
			if(p->d_type == 4)	//是目录文件，深度搜素
			{
				//更新搜索路径，进入下一个目录
				sprintf(buff, "%s%s%s", path, "/", p->d_name);
				printdir(buff, width + 4);
			}
		}
	}
	closedir(dir);
}

int main(int argc, char **argv)
{
	argc_check(argc, 2);
	puts(argv[1]);	//打印当前路径
	printdir(argv[1], 4);
	return 0;
}
