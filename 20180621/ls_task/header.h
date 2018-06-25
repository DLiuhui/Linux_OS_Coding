#ifndef HEADER_H_
#define HEADER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#define argc_check(argc, state) {\
	if(argc != state)\
	{\
		perror("argc error");\
		return -1;\
	}\
}

#define dir_check(dir) {\
	if(!dir)\
	{\
		perror("DIR");\
		return -1;\
	}\
}

#endif

