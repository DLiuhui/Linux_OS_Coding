//利用广度优先算法遍历
#include "header.h"

typedef struct node
{
	char name[256];	//记录文件名
	unsigned char type;	//记录文件类型
	char pwd[512];	//记录文件路径
	unsigned int height; //记录文件层数
	struct node *next;
}node, *List;

int isOverFlow(node *p){
	if(p)
		return 0;
	else
		return 1;
}

//采用尾插法，构成队列形式
List listTailInsert(List *head, List *rear, char name[], unsigned char type, char pwd[], unsigned int height)
{
	node *n = (node*)malloc(sizeof(node));
	if(isOverFlow(n))
	{
		printf("Overflow.\n");
		return *head;
	}
	n->next = NULL;
	strcpy(n->name, name);
	strcpy(n->pwd, pwd);
	n->type = type;
	n->height = height;
	if(*head == *rear)
	{
		(*head)->next = n;
	}
	(*rear)->next = n;
	*rear = n;
	return *head;
}

void BFS_File(char *path)
{
	
	List head = (node*)malloc(sizeof(node));	//头指针
	if(isOverFlow(head))
		return;
	List rear = head;	//尾指针
	head->next = NULL;
	DIR *dir;
	struct dirent *p;
	listTailInsert(&head, &rear, path, 4, path, 0);	//根目录入队，开始广度优先搜索
	char filePath[512];
	node *ptr;
	while(head->next != NULL)	//如果队列非空
	{
		//出队
		ptr = head->next;
		head->next = ptr->next;
		if(head->next == NULL)
			rear = head;
		//打印
		printf("%*s%s\n", (ptr->height) * 4, "", ptr->name);
		if(ptr->type == 4 && strcmp(ptr->name, ".") && strcmp(ptr->name, ".."))	//是目录文件（非叶节点），并且要忽略 . .. 这两个目录
		{
			dir = opendir(ptr->pwd);
			if(!dir)
			{
				perror("opendir");
				return;
			}
			while((p = readdir(dir)) != NULL)	//遍历子节点
			{
				sprintf(filePath, "%s%s%s", ptr->pwd, "/", p->d_name);	//记录文件所在目录
				listTailInsert(&head, &rear, p->d_name, p->d_type, filePath, ptr->height + 1);	//节点入队
			}
		}
		free(ptr);	//释放节点所占内存
	}
	//打印完毕，释放空间
	free(head);
}

int main(int argc, char **argv)
{
	argc_check(argc, 2);
	BFS_File(argv[1]);
	return 0;
}
