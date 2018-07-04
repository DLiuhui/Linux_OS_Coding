#include "LinkList.h"

//溢出检查
int isOverFlow(node *p){
    if(p)
        return 0;
    else
        return 1;
}
//初始化头指针、尾指针
List InitList(List *head, List *rear){
    *head = (node*)malloc(sizeof(node));
    if(isOverFlow(*head)){
        printf("Overflow.\n");
        exit(1);
    }
    (*head)->next = NULL;
    *rear = (node*)malloc(sizeof(node));
    if(isOverFlow(*head)){
        printf("Overflow.\n");
        exit(1);
    }
    *rear = *head;
    return *head;
}
//头插
List listHeadInsert(List *head, List *rear, dataType i){
    node *p = (node*)malloc(sizeof(node));
    if(isOverFlow(p)){
        printf("No more memory.\n");
        return *head;
    }
    if(*head == *rear){  //假如链表为空
        *rear = p;
    }
    p->data = i;
    p->next = (*head)->next;
    (*head)->next = p;
    return *head;
}
//尾插
List listTailInsert(List *head, List *rear, dataType i){
    node *p = (node*)malloc(sizeof(node));
    if(isOverFlow(p)){
        printf("No more memory.\n");
        return *head;
    }
    if(*head == *rear){ //假如链表为空
        (*head)->next = p;
    }
	p->data = i;
	p->next = NULL;
    (*rear)->next = p;
    *rear = p;
    return *head;
}
//有序插入，由compare函数决定链表递增顺序，newdata对应插入量，data对应原始量
List listSortInsert(List *head, List *rear, dataType i, int (*compare)(dataType newdata, dataType data)) {
	node *p = (node*)malloc(sizeof(node));
	if (isOverFlow(p)) {
		printf("No more memory.\n");
		return *head;
	}
	p->data = i;
	if (*head == *rear) {
		(*head)->next = p;
		p->next = NULL;
		*rear = p;
		return *head;
	}
	node *q = *head;
	while (q->next) {
		if ((*compare)(p->data, q->next->data)) {
			p->next = q->next;
			q->next = p;
			return *head;
		}
		else
			q = q->next;
	}
	if (q == *rear) {
		q->next = p;
		p->next = NULL;
		*rear = p;
	}
	return *head;
}
//删除，根据目标关键字，删除结点
List listDeleteByKey(List *head, List *rear, dataType key, int (*equal)(dataType key, dataType data)) {
	node *p = *head;
	while (p->next) {
		if ((*equal)(key, p->next->data)) {
			deleteNode(&p);
			if (p->next == NULL) {//删除完成后，p为最后一个结点
				*rear = p;
			}
			return *head;
		}
		p = p->next;
	}
	return *head;
}
//删除，删除一定数量的结点
List listDeleteByNum(List *head, List *rear, int num) {
	node *p = *head;
	while (num != 0 && p->next) {
		deleteNode(&p);
		num--;
	}
	if (num != 0) {	//链表被清空
		*rear = *head;
	}
	return *head;
}
//删除目标结点的下一个结点
void deleteNode(node **pNode) {
	if ((*pNode)->next) {
		node *q = (*pNode)->next;
		(*pNode)->next = q->next;
		free(q);
	}
}
//根据关键字找结点并返回节点
node* findNodeByKey(List *head, dataType key, int(*equal)(dataType key, dataType data)) {
	node *p = *head;
	while (p->next) {
		if ((*equal)(p->next->data, key))
			return p->next;
		p = p->next;
	}
	return p->next;
}
//根据数目查找对应节点
node* findNodeByNum(List *head, int num) {
	node *p = *head;
	while (num != 0 && p->next) {
		p = p->next;
		num--;
	}
	if(num == 0) {
		return p;
	}
	else {
		printf("There is not so much nodes.\n");
	}
	return p->next;
}
//打印
void printList(List head){
    int count = 0;
    node *p = head;
    while(p->next){
        p = p->next;
        printf("%d", p->data);
        //输出控制
        count++;
        if(count == 25){
            printf("\n");
            count = 0;
        }
        else
            printf(" ");
    }
}
//清空链表节点，头尾指针保留
List clearList(List *head, List *rear){
    node *p = *head;
    node *q;
    while(p->next){
        q = p->next;
        p->next = q->next;
        free(q);
    }
    *rear = *head;
    return *head;
}
//删除整个链表，包括头尾指针
void deleteList(List *head, List *rear){
	clearList(head, rear);	//清空链表后头指针与尾指针已经重合
    free(*head);	//free头指针后，尾指针指向空间已经清空
}
