cmake_minimum_required(VERSION 2.6)

project(enet)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CC_FLAGS_DEBUG "${CMAKE_CC_FLAGS_DEBUG} -g")
add_definitions(-DHAS_FCNTL -DHAS_POLL -DHAS_SOCKLEN_T)

set(ENET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ENET_INCLUDE_DIR ${ENET_INCLUDE_DIR} PARENT_SCOPE)
include_directories(${ENET_INCLUDE_DIR})
add_library(enet STATIC callbacks.c win32.c unix.c protocol.c peer.c packet.c list.c host.c compress.c)