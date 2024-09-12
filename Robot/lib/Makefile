TARGET=liboledfont.so

MILKV_DUO=/home/welpe/Desktop/Workspace/milkv-duo
SDK = ${MILKV_DUO}/C/sdk
APP = ${MILKV_DUO}/C/app
HOST_TOOLS = ${SDK}/duo-buildroot-sdk/host-tools
TOOLCHAIN_PREFIX = $(HOST_TOOLS)/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-
SYSROOT = ${SDK}/rootfs
ROBOT = ${APP}/Robot

CC = $(TOOLCHAIN_PREFIX)gcc
CFLAGS += -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d
LDFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

ROBOT_INC_PATH = $(ROBOT)/include
CFLAGS += -I$(ROBOT_INC_PATH)	\
          -fPIC
LDFLAGS += -shared  

SOURCE = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SOURCE))  

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(CFLAGS)
	
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	@rm *.o -rf
	@rm $(OBJS) -rf
	@rm $(TARGET)