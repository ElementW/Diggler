cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(lzo C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

set(LZO_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
set(LZO_INCLUDE_DIRS ${LZO_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${LZO_INCLUDE_DIRS})

add_library(lzo STATIC minilzo.c)

set(LZO_LIBRARIES lzo PARENT_SCOPE)
