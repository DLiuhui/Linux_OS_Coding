#include "database_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	database_t db;
//	int ret_num;
	database_init(&db, "root", "loveistrue", "net_disk_project");
	database_login(&db);
	database_select(&db, "*", "DIR", "dir_id=1");
	printf("gg\n");
	db.result = mysql_use_result(db.connect_id);
	printf("haha\n");
	MYSQL_ROW row;
	row = mysql_fetch_row(db.result);
	if(NULL != row)
	{
		int num = mysql_num_fields(db.result);
		do
		{
			 int i;
			 for(i = 0; i < num; i++)
			 {
				 printf("%s ", row[i]);
			 }
			 printf("\n");
		}while((row = mysql_fetch_row(db.result))!=NULL);
	}
	else
	{
		printf("no\n");		
	}
	database_logout(&db);
	return 0;
}
