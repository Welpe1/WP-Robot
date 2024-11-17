MILKV_DUO = /home/welpe/Desktop/Workspace/milkv-duo
DUO_BUILDROOT_SDK=$(MILKV_DUO)/duo-buildroot-sdk
HOST_TOOLS = $(DUO_BUILDROOT_SDK)/host-tools
KERNEL_DIR=$(DUO_BUILDROOT_SDK)/linux_5.10/build/cv1800b_milkv_duo_sd
CURRENT_PATH=$(shell pwd)  

export ARCH:=riscv
export CROSS_COMPILE:=$(HOST_TOOLS)/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-

CC = $(CROSS_COMPILE)gcc

all: 
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) modules
	$(CC) driver_main.c -o driver_main -Wall -pthread -O2 -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d
clean:
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) modules clean
	rm -rf driver_main

obj-m += chardev.o