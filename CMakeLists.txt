cmake_minimum_required(VERSION 2.8)

project(Diggler C CXX)

### Find path to build dir, relative if possible, to symlink resources directories
file(RELATIVE_PATH REL_BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
string(FIND "${REL_BUILD_PATH}" ".." RBP_2DOTPOS)
# If we're going up, use absolute path
if(${RBP_2DOTPOS} EQUAL 0)
  set(PATH_TO_TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})
else()
  file(RELATIVE_PATH PATH_TO_TOPDIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
endif()
message(">> Path to topdir: ${PATH_TO_TOPDIR}")

### Compiler & build options
# Disable narrow convert warnings and disable deprecated GLM degrees usage
add_definitions(-DGLM_FORCE_RADIANS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-narrowing -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # -Wextra")

# Define __FILENAME__ as the currently compiled filename
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
#" (fix buggy syntax highlighting on KDevelop/Kate)

if(NOT CMAKE_BUILD_TYPE) # Defaults to release
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(USE_GCC_LTO FALSE CACHE BOOL "Use GCC LTOs")
endif()

message(">> Using ${CMAKE_BUILD_TYPE} build configuration")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
if(USE_GCC_LTO)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
endif()

### Packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(CXX14)
if (NOT CXX14_FOUND)
  message(FATAL_ERROR "C++14 feature support unavailable.
Please use the latest version of your compiler.

Here are the minimum requirements:
- GCC version 5 (on Debian Jessie, enable Stretch/testing repos and get GCC from there)
- clang version 3.4
- MSVC: wait until Microsoft goes bankrupt, and use another compiler in the meantime
- ICC: ICC 17.0 Beta doesn't yet support all required C++14 features")
endif()
set(CMAKE_CXX_FLAGS "${CXX14_FLAGS} ${CMAKE_CXX_FLAGS}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/enet")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/lzfx")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/etc")
find_package(glm)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(LuaJIT REQUIRED)

find_package(msgpack REQUIRED)
if(NOT msgpack_FOUND)
  message(FATAL_ERROR "Bundled msgpack-c not found. Have you checked out submodules?
git submodule update --init")
endif()

pkg_search_module(EPOXY REQUIRED epoxy)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(
  ${ETC_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIR}
  ${LZFX_INCLUDE_DIRS}
  ${ENET_INCLUDE_DIRS}
  ${EPOXY_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${OPENAL_INCLUDE_DIR}
  ${MSGPACK_INCLUDE_DIR}
)

set(SRCS ";")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(diggler ${SRCS})

target_link_libraries(diggler
  ${ETC_LIBRARIES}
  ${LUA_LIBRARY}
  ${LZFX_LIBRARIES}
  ${ENET_LIBRARIES}
  ${EPOXY_LIBRARIES}
  ${GLFW_LIBRARIES} 
  ${OPENAL_LIBRARY}
  pthread)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${PATH_TO_TOPDIR}/assets" "${PROJECT_BINARY_DIR}/assets")

#install(TARGETS digglerz RUNTIME DESTINATION bin)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generate code documentation with Doxygen" VERBATIM
  )
endif()

file(GLOB_RECURSE FILELIST "*.hpp" "*.cpp")
add_custom_target(files SOURCES ${FILELIST})
