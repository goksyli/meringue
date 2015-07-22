CC=gcc
LIB= -lpthread
OBJS=lightVM.o
OBJS+=util.o


all:
	gcc lightVM.c util.c $(LIB) -o lightVM
.PHONY: all

test_util:
	make -C test/ $@
.PHONY: test_util


%.o:%.c
	$(CC) -c $(INCLUDE) -o $@ $<