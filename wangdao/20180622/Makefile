SRCS:=$(wildcard *.c)
<<<<<<< HEAD
CC:=gcc
CFLAGS:=-Wall -g
TARGETS:=$(SRCS:%.c=%)
all:$(TARGETS)
	@for i in $(TARGETS);do gcc -o $${i} $${i}.c $(CFLAGS);done
=======
CFLAGS :=-Wall -g
TARGETS:=$(SRCS:%.c=%)
all:$(TARGETS)
	@for i in $(TARGETS);
	do gcc -o $${i} $${i}.c;
	done	
>>>>>>> 5313891229429cc343f96c50e2265dfc009faac8
clear:
	rm -rf $(TARGETS)

