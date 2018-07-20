#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/msg.h>
#include <sys/sendfile.h>
#include <mysql/mysql.h>
#include <ctype.h>
#include <shadow.h>
#include <crypt.h>

#define TCP_CAPACITY 1000

#define check_argc(argc, state) {\
	if(argc != state) {\
		printf("argc error");\
		return -1;\
	}\
}

#define check_error(ret_val, ret, fun_name) {\
	if(ret_val == ret) {\
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

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n)) 

#define FILENAME "file"

typedef enum {FALSE, TRUE} Boolean;

#endif
