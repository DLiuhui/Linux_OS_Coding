#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <openssl/md5.h>


#define check_argc(argc, state) {\
	if(argc != state) {\
		printf("argc error\n");\
		return -1;\
	}\
}\

#define check_error(ret_val, ret, fun_name){\
	if(ret_val == ret){\
		perror(fun_name);\
		return -1;\
	}\
}\

#define check_thread_error(ret, func_name) {\
	if(ret != 0) {\
		printf("%s is failed %d\n", func_name, ret);\
		return -1;\
	}\
}\

#define clear_out_stdin(c){\
	while((c = getchar()) != '\n' && c != EOF);\
}\

typedef enum {FALSE, TRUE} Boolean;

#define min(m,n) ((m)<(n) ? (m):(n));
#define max(m,n) ((m)>(n) ? (m):(n));

#endif
