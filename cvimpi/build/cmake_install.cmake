# Install script for directory: /home/welpe/Desktop/Workspace/milkv-duo/cvimpi

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/welpe/Desktop/Workspace/milkv-duo/duo-buildroot-sdk/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libini.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libsns_full.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libsample.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libisp.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libvdec.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libvenc.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libawb.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libae.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libaf.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvi_bin.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvi_bin_isp.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libmisc.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libisp_algo.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libsys.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libvpu.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib" TYPE PROGRAM FILES
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/3rd/libini.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libsns_full.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libsample.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libisp.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libvdec.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libvenc.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libawb.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libae.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libaf.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libcvi_bin.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libcvi_bin_isp.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libmisc.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libisp_algo.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libsys.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/middleware/v2/lib/libvpu.so"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvi_rtsp.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib" TYPE PROGRAM FILES "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/rtsp/lib/libcvi_rtsp.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcnpy.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvikernel.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvimath.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcviruntime.so;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libz.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib" TYPE PROGRAM FILES
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/tpu/lib//libcnpy.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/tpu/lib//libcvikernel.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/tpu/lib//libcvimath.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/tpu/lib//libcviruntime.so"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/tpu/lib//libz.so"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libopencv_core.so.3.2;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libopencv_imgcodecs.so.3.2;/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libopencv_imgproc.so.3.2")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib" TYPE PROGRAM FILES
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/opencv/lib//libopencv_core.so.3.2"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/opencv/lib//libopencv_imgcodecs.so.3.2"
    "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/opencv/lib//libopencv_imgproc.so.3.2"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib/libcvi_ive_tpu.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/lib" TYPE PROGRAM FILES "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/3rdparty/ive/lib//libcvi_ive_tpu.so")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/build/3rdparty.out/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/welpe/Desktop/Workspace/milkv-duo/cvimpi/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
