top_srcdir		?= ../../..

include $(top_srcdir)/include/mk/testcases.mk
INSTALL_DIR := testcases/bin/ddt/
INSTALL_MODE := 00755
include $(top_srcdir)/include/mk/generic_leaf_target.mk
gy-521_test : gy-521_test.c
	gcc -g $< -o $@

