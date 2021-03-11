cmake_minimum_required(VERSION 3.0)
project(hwmond LANGUAGES C)

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
else()
    message(FATAL_ERROR "Don't know how to get CPU usage data on this system (you could use -DFAKE=ON)")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/modules")

find_package(LibUSB REQUIRED)

add_executable(hwmond ${SOURCE_FILES})

if (APPLE)
    find_library(CoreFoundation CoreFoundation)
    find_library(IOKit IOKit)
    target_link_libraries(hwmond ${CoreFoundation} ${IOKit} objc)
endif()

include_directories(${LIBUSB_INCLUDE_DIR})
target_link_libraries(hwmond ${LIBUSB_LIBRARY})
target_link_libraries(hwmond m)
target_link_libraries(hwmond pthread)
