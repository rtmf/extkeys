ifneq ($(KERNELRELEASE),) 
# kbuild part of makefile 
obj-m:= c720keys.o
c720keys-y := extkeys.o

else
# normal makefile 
KDIR ?= /lib/modules/`uname -r`/build 

default:
$(MAKE) -C $(KDIR) M=$$PWD
