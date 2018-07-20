#ifndef __R_S_API_H__
#define __R_S_API_H__

#include "header.h"
#include "server.h"

#define check_IO(ret){\
	if(0 == ret){\
		return 0;\
	}else if(-1 == ret){\
		return -1;\
	}\
}\

/*int send_file*/
int send_n(int, char*, int);
int recv_n(int, char*, int);

#endif
