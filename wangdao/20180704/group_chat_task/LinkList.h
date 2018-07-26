#ifndef LINKLIST_H_
#define LINKLIST_H_
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
//链表模板
//根据实际情况
//修改dataType
//修改compare函数
//修改equal函数
//修改printList里的输出控制

typedef struct dataType{
	int client_fd;	//客户机描述符
	char name[20];	//客户名字
}dataType;

typedef struct node {
    struct dataType data;
    struct node *next;
}node, *List;

//溢出检查
int isOverFlow(node *p);
//初始化头指针、尾指针
List InitList(List *head, List *rear);
//头插
List listHeadInsert(List *head, List *rear, dataType i);
//尾插
List listTailInsert(List *head, List *rear, dataType i);
//有序插入，由compare函数决定链表递增顺序，newdata对应插入量，data对应原始量
List listSortInsert(List *head, List *rear, dataType i, int(*compare)(dataType newdata, dataType data));
//删除，根据目标关键字，删除结点
List listDeleteByKey(List *head, List *rear, dataType key, int(*equal)(dataType key, dataType data));
//删除，删除一定数量的结点
List listDeleteByNum(List *head, List *rear, int num);
//删除目标结点的下一个结点
void deleteNode(node **pNode);
//根据关键字找结点并返回节点
node* findNodeByKey(List *head, dataType key, int(*equal)(dataType key, dataType data));
//根据数目查找对应节点
node* findNodeByNum(List *head, int num);
//打印
void printList(List head);
//清空链表节点，头尾指针保留
List clearList(List *head, List *rear);
//删除整个链表，包括头尾指针
void deleteList(List *head, List *rear);
#endif
