SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=main
CC:=gcc
CFLAGS:=-g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@
<<<<<<< HEAD
clear:
=======
clean:
>>>>>>> 5313891229429cc343f96c50e2265dfc009faac8
	rm -rf $(OBJS) $(ELF)

