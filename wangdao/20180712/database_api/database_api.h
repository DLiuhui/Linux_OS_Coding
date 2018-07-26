#ifndef __DATABASE_API__
#define __DATABASE_API__

#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_SET "utf8"
#define DB_HOST "localhost"

#define check_mysql_error(ret_num, arg, func_name){\
	if(ret_num != arg){\
		printf("%s error\n", func_name);\
		return -1;\
	}\
}\

typedef struct database_handle
{
	MYSQL *connect_id;	//数据库连接id
	MYSQL_RES *result;	//数据库的查询结果
	char *password;	//密码
	char *user_name;	//用户名
	char *database_name;	//数据库名
}database_t, *pdatabase_t;

//登录数据库
int database_init(pdatabase_t, char*, char*, char*);
int database_login(pdatabase_t);
int database_logout(pdatabase_t);
int database_add(pdatabase_t, char*, char*, char*);	//增
int database_delete(pdatabase_t, char*, char*);	//删
int database_update(pdatabase_t, char*, char*, char*);	//改
int database_select(pdatabase_t, char*, char*, char*);	//查
#endif
