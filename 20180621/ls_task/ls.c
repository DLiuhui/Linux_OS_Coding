#include "ls.h"
//使用链表，进行文件排序
//比较
int compare(dataType newD, dataType oldD)
{
	if(strcmp(newD.name, oldD.name) < 0)
		return 1;
	else
		return 0;
}
//获得文件名
void getFileName(char *path, char *name)
{
	int i = strlen(path) - 1;
	int j = 0;
	while(path[i] != '/' && i != 0)
	{
		i--;
	}
	if(i != 0)
		i++;
	while(path[i] != '\0')
	{
		name[j++] = path[i++];
	}
	name[j] = '\0';
}
//获得权限字符串
void getMode(char *mode, mode_t st_mode)
{
	unsigned int type = 7  & (st_mode >> 13);
	switch(type)
	{
		case 4:	//一般文件
			mode[0] = '-';
			break;
		case 2:	//文件夹文件
			mode[0] = 'd';
			break;
		default:	//其他，暂时不清楚其他文件类型对应的码
			mode[0] = 'K';
			break;
	}
	unsigned int root = 1 << 9;
	int i;
	for(i = 0; i < 3; i++)
	{
		root >>= 1;
		if((root & st_mode) != 0)
			mode[1 + 3 * i] = 'r';
		else
			mode[1 + 3 * i] = '-';
		root >>= 1;
		if((root & st_mode) != 0)
			mode[2 + 3 * i] = 'w';
		else
			mode[2 + 3 * i] = '-';
		root >>= 1;
		if((root & st_mode) != 0)
			mode[3 + 3 * i] = 'x';
		else
			mode[3 + 3 * i] = '-';
	}
	mode[10] = '\0';
}
//打印文件状态
void printFileState(char *name, struct stat s, int sizePrint)
{
	char str[50];
	sprintf(str,"%s",ctime(&(s.st_mtime)));
	char month[4] = {str[4], str[5], str[6], '\0'};
	char day[3] = {str[8], str[9], '\0'};
	char timeN[6] = {str[11], str[12], str[13], str[14], str[15], '\0'};
	char mode[11];
	getMode(mode, s.st_mode);
	off_t fs = s.st_size;
	int plength = 0;
	while(fs != 0)
	{
		plength++;
		fs /= 10;
	}
	printf("%s %ld %s %s %*s%ld %s %s %s %s\n",
		mode, s.st_nlink, getpwuid(s.st_uid)->pw_name, getgrgid(s.st_gid)->gr_name,
		sizePrint - plength, "", s.st_size, month, day, timeN, name);
}
//打印文件表
void printFileList(List head, int sizePrint)
{
    node *p = head;
    while(p->next)
	{
        p = p->next;
        //输出控制
		printFileState(p->data.name, p->data.state, sizePrint);
	}
}
//实现ls
void ls(char *path)
{
	DIR *dir;	//目录指针
	int ret;
	struct stat fileState;	//文件状态结构体
	struct dirent *p;
	dir = opendir(path);
	if(!dir)	//地址无效，但也有可能路径为文件
	{
		//判断路径是否为文件
		if((ret = stat(path, &fileState)) != -1)
		{
			//路径为文件
			//从path中获取文件名字
			char fileName[512] = {0};
			getFileName(path, fileName);
			printFileState(fileName, fileState, 0);
			return;
		}
		else	//路径错误
		{
			perror("opendir");
			return;
		}
	}
	//若路径指向目录，将目录下的文件按照文件名升序的顺序存入
	//然后再打印
	List head = NULL, rear = NULL;
	InitList(&head, &rear);
	char filePath[512] = {0};
	dataType data;
	off_t sizeFile = 0;
	while((p = readdir(dir)) != NULL)
	{
		sprintf(filePath, "%s%s%s", path, "/", p->d_name);
		ret = stat(filePath, &fileState);
		if(-1 == ret)
		{
			perror("stat");
			return;
		}
		strcpy(data.name, p->d_name);
		data.state = fileState;
		if(fileState.st_size > sizeFile)	//存储最大的文件大小
			sizeFile = fileState.st_size;
		listSortInsert(&head, &rear, data, &compare);	//排序存入链表
	}
	int sizePrint = 0;	//控制打印时文件大小大位置参数
	while(sizeFile != 0)
	{
		sizePrint++;
		sizeFile /= 10;
	}
	printFileList(head, sizePrint);
	deleteLsit(&head, &rear);
}
