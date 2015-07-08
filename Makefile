all:
	gcc lightVM.c -o lightVM
.PHONY: all

test_util:
	make -C test/ $@
.PHONY: test_util