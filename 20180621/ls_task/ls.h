#ifndef LS_H_
#define LS_H_
#include "header.h"
#include "LinkList.h"

//比较
int compare(dataType newD, dataType oldD);
//获得文件名
void getFileName(char *path, char *name);
//获得权限字符串
void getMode(char *mode, mode_t st_mode);
//打印文件状态
void printFileState(char *name, struct stat s, int sizePrint);
//打印文件表
void printFileList(List head, int sizePrint);
//实现ls
void ls(char *path);
#endif