cmake_minimum_required(VERSION 2.6)

project(ETC C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_RELEASE} -O3")

set(ETC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
set(ETC_INCLUDE_DIRS ${ETC_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${ETC_INCLUDE_DIRS})

add_library(stb_image STATIC stb_image.c)
add_library(stb_perlin STATIC stb_perlin.c)
add_library(stb_vorbis STATIC stb_vorbis.c)

set(ETC_LIBRARIES stb_image stb_perlin stb_vorbis PARENT_SCOPE)