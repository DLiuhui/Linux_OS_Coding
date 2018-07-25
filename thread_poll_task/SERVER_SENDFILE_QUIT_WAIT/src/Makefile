SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=SERVER
CC:=gcc
CFLAGS:=-Wall -g -I../include
$(ELF):$(OBJS)
	gcc $^ -o $@ -lpthread
clean:
	rm -rf $(OBJS) $(ELF)

