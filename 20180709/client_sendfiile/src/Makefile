SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=CLIENT
CC:=gcc
CFLAGS:=-g -Wall -I../include
$(ELF):$(OBJS)
	gcc $^ -o $@
clean:
	rm -rf $(OBJS) $(ELF)

