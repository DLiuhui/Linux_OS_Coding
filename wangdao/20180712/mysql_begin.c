#include "header.h"

int main(int argc, char **argv)
{
	check_argc(argc, 2);
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *server = "localhost";
	char *user = "root";
	char *password = "loveistrue";
	char *database = "test25";
	char query[300] = "select * from student where id=";
	sprintf(query, "%s%s", query, argv[1]);
	//puts(query);
	int ret, i, item_num;
	conn = mysql_init(NULL);
	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		printf("Error connecting to database:%s\n", mysql_error(conn));
		return -1;
	}else{
		printf("Connected\n");
	}
	if(mysql_set_character_set(conn, "utf8"))	//设置mysql传输编码，防止中文字符打印时编程乱码
	{
		printf("set character error\n");
		mysql_close(conn);	//关闭连接
		return -1;
	}
	ret = mysql_query(conn, query);
	if(ret)
	{
		printf("Error making query:%s\n", mysql_error(conn));
	}else{
		res = mysql_use_result(conn);
		if(res)
		{
			while((row = mysql_fetch_row(res)) != NULL)
			{
				item_num = mysql_num_fields(res);
				for(i = 0; i < item_num; i++)
				{
					printf("%-8s", row[i]);
				}
				printf("\n");
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);	//关闭连接
	return 0;
}
