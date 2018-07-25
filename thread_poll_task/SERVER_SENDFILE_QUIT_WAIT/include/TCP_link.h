#ifndef __TCP_LINK_H__
#define __TCP_LINK_H__
#include "header.h"

#define check_TCP(ret_num, ret, fun_name){\
	if(ret_num == ret){\
		printf("%s error\n", fun_name);\
		return -1;\
	}\
}\
//服务器建立TCP连接
//socket bind listen
int TCP_link_server(int*, char*, char*);
//socket connect
//int TCP_link_client(int*, char*, char*);

#endif

