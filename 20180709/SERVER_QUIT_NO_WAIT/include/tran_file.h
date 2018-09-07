#ifndef __TRAN_FILE_H__
#define __TRAN_FILE_H__
#include "header.h"

typedef struct{
	int len;
	char buf[1000];
}Train;

int tran_file(int, char*);
int send_n(int, char*, int);
int recv_n(int, char*, int);

#endif
