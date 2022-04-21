obj-m := ioctl_htc.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
 
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order .*.cmd *.mod