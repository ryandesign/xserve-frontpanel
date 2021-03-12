cmake_minimum_required(VERSION 3.1)
project(hwmond LANGUAGES C)

option(ESXI "Build for VMware ESXi (on ELF-based OS only)")
option(FAKE "Use fake (random) CPU usage data")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(SOURCE_FILES hwmond.c cpu_usage.c)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(mach/mach_host.h HAVE_MACH_MACH_HOST_H)

if(FAKE)
    message(NOTICE "Using fake (random) CPU usage data")
    list(APPEND SOURCE_FILES cpu_usage_random.c)
elseif(HAVE_MACH_MACH_HOST_H)
    list(APPEND SOURCE_FILES cpu_usage_mach.c)
elseif(EXISTS "/proc/stat")
    list(APPEND SOURCE_FILES cpu_usage_procfs.c)
else()
    message(FATAL_ERROR "Don't know how to get CPU usage data on this system (you could use -DFAKE=ON)")
endif()

if(ESXI)
    if(APPLE OR NOT UNIX)
        message(FATAL_ERROR "Can only build for VMware ESXi on ELF-based UNIX operating systems like Linux")
    endif()
    string(APPEND CMAKE_C_FLAGS " -m32")
    set(ESXI_LIBUSB_LIBRARY "libusb-1.0.so.0.1.0")
    set(LIBUSB_LIBRARY ${CMAKE_SOURCE_DIR}/${ESXI_LIBUSB_LIBRARY})
    if(NOT EXISTS ${LIBUSB_LIBRARY})
        message(FATAL_ERROR "Copy ${ESXI_LIBUSB_LIBRARY} from the ESXi host's /lib directory into the source directory")
    endif()
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/modules")

find_package(LibUSB REQUIRED)

add_executable(hwmond ${SOURCE_FILES})
set_property(TARGET hwmond PROPERTY C_STANDARD 99)

if (APPLE)
    find_library(CoreFoundation CoreFoundation)
    find_library(IOKit IOKit)
    target_link_libraries(hwmond ${CoreFoundation} ${IOKit} objc)
endif()

include_directories(${LIBUSB_INCLUDE_DIR})
target_link_libraries(hwmond ${LIBUSB_LIBRARY})
target_link_libraries(hwmond m)
target_link_libraries(hwmond pthread)
