all:
	gcc lightVM.c util.c -o lightVM
.PHONY: all

test_util:
	make -C test/ $@
.PHONY: test_util