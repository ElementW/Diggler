cmake_minimum_required(VERSION 2.6)

project(ENET C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

add_definitions(-DHAS_FCNTL -DHAS_POLL -DHAS_SOCKLEN_T)

set(ENET_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ENET_INCLUDE_DIRS ${ENET_INCLUDE_DIRS} PARENT_SCOPE)

add_library(enet STATIC callbacks.c win32.c unix.c protocol.c peer.c packet.c list.c host.c compress.c)
target_include_directories(enet PUBLIC ${ENET_INCLUDE_DIRS})

set(ENET_LIBRARIES enet PARENT_SCOPE)