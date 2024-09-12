
TARGET = main
#cv180x(64) cv181x(256)
CHIP ?= cv181x

MILKV_DUO = /home/welpe/Desktop/Workspace/milkv-duo
SDK = $(MILKV_DUO)/C/sdk
APP = $(MILKV_DUO)/C/app
HOST_TOOLS = $(SDK)/duo-buildroot-sdk/host-tools
SYSROOT = $(SDK)/rootfs
ROBOT = $(APP)/Robot

ifeq ($(CHIP),cv180x)
	USE_TPU_IVE=ON
    CVI_TDL_SDK=${SDK}/cvitek-tdl-sdk-cv180x
else ifeq ($(CHIP), cv181x)  
	USE_TPU_IVE=OFF
    CVI_TDL_SDK=${SDK}/cvitek-tdl-sdk-sg200x
endif

MW_PATH=$(CVI_TDL_SDK)/sample/3rd/middleware/v2
TPU_PATH=$(CVI_TDL_SDK)/sample/3rd/tpu
IVE_PATH=$(CVI_TDL_SDK)/sample/3rd/ive

CROSS_COMPILE = ${HOST_TOOLS}/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip

#Wiringx stuffs
WIRINGX_LIB_PATH = $(SYSROOT)/usr/lib
WIRINGX_INC_PATH = $(SYSROOT)/usr/include
WIRINGX_LIBS = -lwiringx

#Robot stuffs
ROBOT_LIB_PATH = $(ROBOT)/lib
ROBOT_INC_PATH = $(ROBOT)/include
ROBOT_OLED_LIBS = -loledfont

# Middleware stuffs
MW_LIB_PATH = $(MW_PATH)/lib
MW_INC_PATH = $(MW_PATH)/include
MW_PANEL_INC_PATH += ${MW_PATH}/include/panel
MW_SAMPLE_PATH = $(MW_PATH)/sample/common
MW_3RD_LIB_PATH = $(MW_LIB_PATH)/3rd
MW_AUDIO_LIBS = -lcvi_audio -lcvi_vqe -lcvi_ssp -ltinyalsa -lcvi_VoiceEngine -lcvi_RES1 -lcli
MW_LIBS = -lini -lsns_full -lsample -lisp -lvdec -lvenc -lawb -lae -laf -lcvi_bin -lcvi_bin_isp -lmisc -lisp_algo -lsys -lvpu

ifeq ($(CHIP),cv180x)
    MW_ISP_INC_PATH = $(MW_INC_PATH)/isp/cv180x
	CFLAGS += -DCV180X
else ifeq ($(CHIP), cv181x)  
    MW_ISP_INC_PATH = $(MW_INC_PATH)/isp/cv181x
	CFLAGS += -DCV181X
endif

# TDL SDK stuffs
SDK_ROOT_PATH ?= $(CVI_TDL_SDK)
SDK_LIB_PATH = $(SDK_ROOT_PATH)/lib
SDK_INC_PATH = $(SDK_ROOT_PATH)/include
SDK_TDL_INC_PATH = $(SDK_ROOT_PATH)/include/cvi_tdl
SDK_APP_INC_PATH = $(SDK_ROOT_PATH)/include/cvi_tdl_app
SDK_SAMPLE_INC_PATH = $(SDK_ROOT_PATH)/sample/3rd/include
SDK_SAMPLE_UTILS_PATH = $(SDK_ROOT_PATH)/sample/utils
SDK_TDL_LIBS = -lcvi_tdl
SDK_APP_LIBS = -lcvi_tdl_app

# Opencv stuffs
OPENCV_INC_PATH = $(SDK_ROOT_PATH)/sample/3rd/opencv/include
OPENCV_LIB_PATH = $(SDK_ROOT_PATH)/sample/3rd/opencv/lib
OPENCV_LIBS = -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

# TPU stuffs
TPU_LIB_PATH = $(TPU_PATH)/lib
TPU_LIBS = -lcnpy  -lcvikernel  -lcvimath  -lcviruntime  -lz -lm

# IVE stuffs
ifeq ($(USE_TPU_IVE), ON)
	IVE_SDK_LIB_PATH = $(IVE_PATH)/lib
	IVE_SDK_INC_PATH = $(IVE_PATH)/include
	IVE_LIBS = -lcvi_ive_tpu
	CFLAGS += -DUSE_TPU_IVE
else
	IVE_SDK_LIB_PATH = $(MW_PATH)/lib
	IVE_SDK_INC_PATH = $(MW_PATH)/include
	IVE_LIBS = -lcvi_ive
endif


# RTSP stuffs
RTSP_LIB_PATH = $(SDK_ROOT_PATH)/sample/3rd/rtsp/lib
RTSP_INC_PATH = $(SDK_ROOT_PATH)/sample/3rd/rtsp/include/cvi_rtsp
RTSP_LIBS = -lcvi_rtsp

# STB stuffs
STB_INC_PATH = $(SDK_ROOT_PATH)/sample/3rd/stb/include

# CVI LIB stuffs
MD_INC_PATH = $(SDK_ROOT_PATH)/include/cvi_md
DRAW_RECT_INC_PATH = $(SDK_ROOT_PATH)/include/cvi_draw_rect
PREPROCESS_INC_PATH = $(SDK_ROOT_PATH)/include/cvi_preprocess

ROBOT_LIBS = $(ROBOT_OLED_LIBS) -L$(ROBOT_LIB_PATH)
SYSROOT_LIBS = ${WIRINGX_LIBS} -L${WIRINGX_LIB_PATH}
SAMPLE_BASE_LIBS = -L$(TPU_LIB_PATH) $(TPU_LIBS) -L$(IVE_SDK_LIB_PATH) $(IVE_LIBS) -L$(SDK_LIB_PATH) $(SDK_TDL_LIBS)
SAMPLW_MW_LIB = -L$(MW_LIB_PATH) -L$(MW_3RD_LIB_PATH) $(MW_LIBS)
SAMPLE_OPENCV_LIB = -L$(OPENCV_LIB_PATH) $(OPENCV_LIBS)
SAMPLE_INIT_LIBS = $(SAMPLW_MW_LIB) $(SAMPLE_BASE_LIBS)
SAMPLE_AUD_LIBS = $(SAMPLW_MW_LIB) $(SAMPLE_BASE_LIBS)  $(MW_AUDIO_LIBS)
SAMPLE_YOLO_LIBS = $(SAMPLW_MW_LIB) $(SAMPLE_BASE_LIBS)  $(SAMPLE_OPENCV_LIB)
SAMPLE_VI_LIBS = $(SAMPLW_MW_LIB) $(SAMPLE_OPENCV_LIB) $(SAMPLE_BASE_LIBS) -L$(RTSP_LIB_PATH) $(RTSP_LIBS)
SAMPLE_APP_LIBS = $(SAMPLW_MW_LIB) $(SAMPLE_OPENCV_LIB) $(SAMPLE_BASE_LIBS) -L$(RTSP_LIB_PATH) $(RTSP_LIBS) $(SDK_APP_LIBS)


LDFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -O3 -DNDEBUG \
		  -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d \
		  -D_MIDDLEWARE_V2_ \
		  -std=gnu11 -Wno-pointer-to-int-cast -fsigned-char -Wno-format-truncation -fdiagnostics-color=always -s -lpthread -latomic \
	

CFLAGS += -I$(SDK_INC_PATH) \
          -I$(SDK_TDL_INC_PATH) \
          -I$(SDK_APP_INC_PATH) \
          -I$(SDK_SAMPLE_INC_PATH) \
		  -I$(SDK_SAMPLE_UTILS_PATH) \
          -I$(RTSP_INC_PATH) \
          -I$(IVE_SDK_INC_PATH) \
          -I$(OPENCV_INC_PATH) \
          -I$(STB_INC_PATH) \
		  -I$(MW_SAMPLE_PATH) \
          -I$(MW_ISP_INC_PATH) \
		  -I$(MW_PANEL_INC_PATH) \
		  -I$(MW_PANEL_BOARD_INC_PATH) \
		  -I$(MW_LINUX_INC_PATH) \
		  -I$(MW_INC_PATH) \
		  -I$(MW_INC_PATH)/linux \
          -I$(AISDK_ROOT_PATH)/include/stb \
		  -I$(WIRINGX_INC_PATH) \
		  -I$(ROBOT_INC_PATH) \



.PHONY : all clean
all: $(TARGET)
clean:
	@rm *.o -rf
	@rm $(OBJS) -rf
	@rm $(TARGET)

#当前文件夹下的所有.c文件
OBJS_SRC = $(wildcard $(PWD)/*.c)  
OBJS = $(OBJS_SRC:.c=.o)
$(OBJS): %.o: %.c  
	$(CC) $(CFLAGS) -o $@ -c $< 

$(SDK_ROOT_PATH)/sample/utils/%.o: $(SDK_ROOT_PATH)/sample/utils/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

#依赖项
OBJS+=$(SDK_ROOT_PATH)/sample/utils/sample_utils.o \
		$(SDK_ROOT_PATH)/sample/utils/middleware_utils.o \
        $(SDK_ROOT_PATH)/sample/utils/vi_vo_utils.o \

LIBS+=$(SAMPLE_VI_LIBS) \
		${SYSROOT_LIBS}	\
		${ROBOT_LIBS}	\
		${SAMPLE_APP_LIBS} \

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^



