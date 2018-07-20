SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=net_disk_client
CC:=gcc
CFLAGS:=-g -Wall -I../include
$(ELF):$(OBJS)
	gcc $^ -o $@ -lssl -lcrypto
clean:
	rm -rf $(OBJS) $(ELF)

