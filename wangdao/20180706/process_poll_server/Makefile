SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=SERVER
CC:=gcc
CFLAGS:=-g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@
clean:
	rm -rf $(OBJS) $(ELF)

