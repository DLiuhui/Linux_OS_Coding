#include "header.h"

int printFile(char *path, char *fileName)
{
	//深度优先搜索查找文件
	DIR *dir;
	dir = opendir(path);
	dir_check(dir);
	struct dirent *p;
	char buf[512] = {0};
	while((p = readdir(dir)) != NULL)
	{
		if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			;
		else if(!strcmp(p->d_name, fileName))
		{
			printf("%s\n", path);
			return 1;
		}
		else if(p->d_type == 4)
		{
			sprintf(buf, "%s%s%s", path, "/", p->d_name);
			return printFile(buf, fileName);
		}
	}
	closedir(dir);
	return 0;
}

int main(int argc, char **argv)
{
	//输入路径与文件名
	argc_check(argc, 3);
	if(!printFile(argv[1], argv[2]))
	{
		printf("Not found!\n");
	}
	return 0;
}
