#ifndef LINKLIST_H_
#define LINKLIST_H_
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

//链表模板
//根据实际情况
//修改dataType
//修改compare函数
//修改equal函数
//修改printList里的输出控制

typedef struct dataType {
    struct stat state;
    char name[256];
}dataType;

typedef struct node {
    dataType data;
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
//清空链表节点，头尾指针保留
List clearList(List *head, List *rear);
//删除整个链表，包括头尾指针
void deleteLsit(List *head, List *rear);
#endif