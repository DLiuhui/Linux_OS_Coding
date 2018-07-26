#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define check_argc(argc, state) {\
	if(argc != state) {\
		printf("argc error");\
		exit(-1);\
	}\
}

#define check_error(ret_val, ret, fun_name) {\
	if(ret_val == ret) {\
		perror(fun_name);\
		exit(-1);\
	}\
}\

#define check_thread_error(ret, fun_name){\
	if(ret != 0){\
		printf("%s is failed, ret = %d\n", func_name, ret);\
		return -1;\
	}\
}\

typedef enum {FALSE, TRUE} Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n)) 
#endif
