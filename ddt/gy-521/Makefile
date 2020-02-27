top_srcdir		?= ../../..

include $(top_srcdir)/include/mk/testcases.mk
INSTALL_DIR := testcases/bin/ddt/
INSTALL_MODE := 00755
include $(top_srcdir)/include/mk/generic_leaf_target.mk
rpi-mpu6050 : rpi-mpu6050.c
	gcc -g $< -o $@

clean:
	rm rpi-mpu6050
