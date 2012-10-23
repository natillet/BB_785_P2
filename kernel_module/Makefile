obj-m := enable_usermode.o

#KDIR := /lib/modules/$(shell uname -r)/build
#KDIR := /usr/src/linux-3.2.30
KDIR := /usr/src/kernel

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

