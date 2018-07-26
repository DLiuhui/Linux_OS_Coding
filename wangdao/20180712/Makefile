SRCS:=$(wildcard *.c)
CC:=gcc
TARGETS:=$(SRCS:%.c=%)
CFLAGS:=-Wall -g
$(TARGETS):
	@for i in $(TARGETS);do gcc $${i}.c -o $${i} -lmysqlclient;done
#	gcc $^ -o $@ -lmysqlclient
#	gcc query.c -o query_mysql -I /usr/include/mysql/ -lmysqlclient
#	gcc delete.c -o delete_mysql -I /usr/include/mysql/ -lmysqlclient
#	gcc insert.c -o insert_mysql -I /usr/include/mysql/ -lmysqlclient
#	gcc update.c -o update_mysql -I /usr/include/mysql/ -lmysqlclient
clean:
	rm -rf $(TARGETS).o
clean_all:
	rm -rf $(TARGETS).o $(TARGETS)
