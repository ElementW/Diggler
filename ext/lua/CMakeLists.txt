cmake_minimum_required(VERSION 2.6)

project(LUA)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX")
endif()

set(LUA_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIRS} PARENT_SCOPE)

add_library(lua STATIC lapi.c lauxlib.c lbaselib.c lcode.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c lvm.c lzio.c print.c)
target_include_directories(lua PUBLIC ${LUA_INCLUDE_DIRS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(lua dl)
endif()

set(LUA_LIBRARIES lua PARENT_SCOPE)