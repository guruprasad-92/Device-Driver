TARGET_MODULE := gsm_module
KDIR := /lib/modules/$(shell uname -r)/build/
obj-m := GSM.o
GSM-objs += device.o

all : 
# run kernel build system to make module
	$(MAKE) -C ${KDIR} M=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C ${KDIR} M=$(PWD) clean
