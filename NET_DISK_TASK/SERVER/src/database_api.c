#include "database_api.h"

//用户名，密码，数据库库名字
int database_init(pdatabase_t db, char *user_name, char *password, char *database_name)
{
	db->connect_id = mysql_init(NULL);
	db->user_name = (char*)calloc(sizeof(user_name), sizeof(char));
	strcpy(db->user_name, user_name);
	db->password = (char*)calloc(sizeof(password), sizeof(char));
	strcpy(db->password, password);
	db->database_name = (char*)calloc(sizeof(database_name), sizeof(char));
	strcpy(db->database_name, database_name);
	return 1;
}

int database_login(pdatabase_t db)
{
	int ret_num;
	if(!mysql_real_connect(db->connect_id, DB_HOST, db->user_name, db->password, db->database_name, 0, NULL, 0))
	{
		printf("database connect failure\n");
		return -1;
	}
#ifdef CHARACTER_SET
	ret_num = mysql_set_character_set(db->connect_id, CHARACTER_SET);
	check_mysql_error(0, ret_num, "mysql_set_character_set");
#endif
	return 1;
}

int database_logout(pdatabase_t db)
{
	//mysql_free_result(db->result);
	free(db->password);
	free(db->user_name);
	free(db->database_name);
	mysql_close(db->connect_id);
	return 1;
}

int database_add(pdatabase_t db, char *table_name, char *items, char *values)
{
	char buffer[500];
	int ret_num;
	sprintf(buffer, "INSERT INTO %s (%s) values(%s)", table_name, items, values);
	ret_num = mysql_query(db->connect_id, buffer);
	check_mysql_error(0, ret_num, "mysql_query");
	return 1;
}

int database_delete(pdatabase_t db, char *table_name, char *where)
{
	char buffer[500];
	int ret_num;
	sprintf(buffer, "DELETE FROM %s WHERE %s", table_name, where);
	//printf("%s\n", buffer);
	ret_num = mysql_query(db->connect_id, buffer);
	check_mysql_error(0, ret_num, "mysql_query");
	return 1;
}

int database_update(pdatabase_t db, char *table_name, char *set, char *where)
{
	char buffer[500];
	int ret_num;
	sprintf(buffer, "UPDATE %s SET %s WHERE %s", table_name, set, where);
	ret_num = mysql_query(db->connect_id, buffer);
	check_mysql_error(0, ret_num, "mysql_query");
	return 1;
}

int database_select(pdatabase_t db, char *show_items, char *table_name, char *where)
{
	char buffer[500];
	int ret_num;
	if(NULL == where)
	{
		sprintf(buffer, "SELECT %s FROM %s", show_items, table_name);
	}
	else
	{
		sprintf(buffer, "SELECT %s FROM %s WHERE %s", show_items, table_name, where);
	}
	//printf("%s\n", buffer);
	ret_num = mysql_query(db->connect_id, buffer);
	check_mysql_error(0, ret_num, "mysql_query");
	return 1;
}
