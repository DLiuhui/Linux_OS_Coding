SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=net_disk_server
CC:=gcc
CFLAGS:=-Wall -g -I../include
$(ELF):$(OBJS)
	gcc $^ -o $@ -lpthread -lmysqlclient -lcrypt
clean:
	rm -rf $(OBJS) $(ELF)

