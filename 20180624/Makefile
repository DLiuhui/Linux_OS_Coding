SRCS:=$(wildcard *.c)
CC:=gcc
CFLAGS:=-Wall -g
TARGETS:=$(SRCS:%.c=%)
all:$(TARGETS)
	@for i in $(TARGETS);do gcc -o $${i} $${i}.c $(CFLAGS);done
clear:
	rm -rf $(TARGETS)

