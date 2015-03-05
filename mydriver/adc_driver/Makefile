ifneq ($(KERNELRELEASE),)

obj-m := mini6410_adc.o

else
	
KDIR := /opt/FriendlyARM/mini6410/linux/linux-2.6.38
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*

endif
