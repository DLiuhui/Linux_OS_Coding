<<<<<<< HEAD
SRCS:=$(wildcard *.c)
CC:=gcc
CFLAGS:=-g -Wall
TARGETS:=$(SRCS:%.c=%)
all:$(TARGETS)
	@for i in $(TARGETS);do gcc -o $${i} $${i}.c $(CFLAGS);done
clear:
	rm -rf $(TARGETS)
=======
SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=main
CC:=gcc
CFLAGS:=-g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@
clean:
	rm -rf $(OBJS) $(ELF)
>>>>>>> 5313891229429cc343f96c50e2265dfc009faac8

