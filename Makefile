all:
	gcc lightVM.c -o lightVM


test_util:
	make -C test/ $@