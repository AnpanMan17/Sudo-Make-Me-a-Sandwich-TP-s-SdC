obj-m += sdec_prueba.o

# Apunta directo al directorio v8 de home local, donde se encuentran los headers del kernel para Raspberry Pi
KDIR := $(HOME)/rpi_kernel/linux-headers-6.12.75+rpt-rpi-v8

ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

all:
	make -C $(KDIR) M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean