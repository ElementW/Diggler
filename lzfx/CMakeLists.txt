cmake_minimum_required(VERSION 2.6)

project(lzfx)
set(LZFX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LZFX_INCLUDE_DIR ${LZFX_INCLUDE_DIR} PARENT_SCOPE)
add_library(lzfx STATIC lzfx.c)