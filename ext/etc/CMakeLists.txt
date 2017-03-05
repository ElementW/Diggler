cmake_minimum_required(VERSION 2.6)

project(ETC C CXX)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

set(ETC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
set(ETC_INCLUDE_DIRS ${ETC_INCLUDE_DIRS} PARENT_SCOPE)
include_directories(${ETC_INCLUDE_DIRS})

add_library(stb_c_lexer STATIC stb_c_lexer.c)

add_library(stb_image STATIC stb_image.c)
target_compile_definitions(stb_image PUBLIC -DSTBI_ONLY_PNG -DSTBI_ONLY_JPEG)

add_library(stb_perlin STATIC stb_perlin.c)

add_library(stb_vorbis STATIC stb_vorbis.c)

add_library(simplexnoise STATIC simplexnoise.cpp)


set(ETC_LIBRARIES stb_c_lexer stb_image stb_perlin stb_vorbis simplexnoise PARENT_SCOPE)
