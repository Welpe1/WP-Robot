cmake_minimum_required(VERSION 3.5)
project(main)
set(CMAKE_CXX_STANDARD 11)
set(PROJECT_NAME main)

set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk")
set(SYSROOT "${SDK_DIR}/rootfs")
set(CMAKE_C_COMPILER "${SDK_DIR}/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc")
set(CMAKE_CXX_COMPILER "${SDK_DIR}/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-g++")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=c906fdv -march=rv64imafdcv0p7xthead -mcmodel=medany -mabi=lp64d")

set(OpenCV_DIR "${SDK_DIR}/opencv-mobile-4.10.0-milkv-duo/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})	
	
target_include_directories(${PROJECT_NAME} PRIVATE ${SYSROOT}/usr/include) 
find_library(WIRINGX libwiringx.so PATHS ${SYSROOT}/usr/lib)  
target_link_libraries(${PROJECT_NAME} ${WIRINGX})


