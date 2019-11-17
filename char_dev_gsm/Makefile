TARGET := gsm

#KDIR := /lib/modules/$(shell uname -r)/build/
KDIR := /home/guru/OFC/lnx/projects/V4/source_codes/git/linux-at91/
obj-m := ${TARGET}.o #Object name should not be match with the main file name 
# This causes "missing MODULE_LICENSE()" warnig while building the module
#${TARGET}-objs := lkm_gsm
include ${PWD}/src/src.mk
export CFLAGS := -std=c99


all :
# run kernel build system to make module
	$(MAKE) -C ${KDIR} M=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C ${KDIR}  M=$(PWD) clean
