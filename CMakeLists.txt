cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(Diggler C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  # Forbid in-tree builds. With CMake, doing an in-tree build breaks out-of-tree ones
  # when generating from .in files into the source directory, overriding the path of out-of-tree
  # generated ones.
  # TL;DR: in-tree build = BAD IDEA.
  file(REMOVE_RECURSE CMakeFiles CMakeCache.txt)
  message(FATAL_ERROR
    "You are doing an in-tree build (i.e. the source and binary directories are the same).\
In-tree builds aren't supported and will *never* be.\
Make a \"build\" directory and run cmake here.
\
Don't forget to remove CMakeFiles and CMakeCache.txt.")
endif()

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
if(MINGW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -municode -mwindows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -municode -mwindows")
  add_definitions(-DHAS_INET_PTON=1) # Override ENet's CMakeFile broken pton/ntop detection
  add_definitions(-DHAS_INET_NTOP=1)
endif()
# Disable narrow convert warnings and disable deprecated GLM degrees usage
add_definitions(-DGLM_FORCE_RADIANS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # -Wextra")

if(NOT CMAKE_BUILD_TYPE) # Defaults to release
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(USE_GCC_LTO FALSE CACHE BOOL "Use GCC LTOs")

  # Compile as Position Independent Code
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  if(NOT (MINGW OR WIN32))
    # On non-Windows platforms, make the whole executable position-independent
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pie")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie")
  endif()
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

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/enet")
set(ENET_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/enet/include")
set(ENET_LIBRARIES enet)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/lzfx")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/etc")
find_package(glm)
find_package(OpenAL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(LuaJIT REQUIRED)

find_package(meiose REQUIRED)
if(NOT MPACK_FOUND)
  message(FATAL_ERROR "Bundled meiose not found. Have you checked out submodules?
git submodule update --init")
endif()

find_package(Optional REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(SQLITE3 REQUIRED sqlite3)
pkg_search_module(LIBSODIUM REQUIRED libsodium)
include_directories(BEFORE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${ETC_INCLUDE_DIRS}
  ${MEIOSE_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${LZFX_INCLUDE_DIRS}
  ${ENET_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${LIBSODIUM_INCLUDE_DIRS}
  ${SQLITE3_INCLUDE_DIRS}
  ${OPENAL_INCLUDE_DIR}
  ${OPTIONAL_INCLUDE_DIR}
)

### Allow subdirs to add params to the `diggler` target
# Sources
set_property(GLOBAL PROPERTY DIGGLER_SOURCE_FILES "")
function(diggler_add_sources)
  get_property(SRCS GLOBAL PROPERTY DIGGLER_SOURCE_FILES)
  list(APPEND SRCS ${ARGV})
  set_property(GLOBAL PROPERTY DIGGLER_SOURCE_FILES "${SRCS}")
endfunction(diggler_add_sources)

# Definitions
set_property(GLOBAL PROPERTY DIGGLER_DEFINITIONS "")
function(diggler_add_definition)
  get_property(DEFS GLOBAL PROPERTY DIGGLER_DEFINITIONS)
  list(APPEND DEFS "${ARGV0}")
  set_property(GLOBAL PROPERTY DIGGLER_DEFINITIONS "${DEFS}")
endfunction(diggler_add_definition)

# Include directories
set_property(GLOBAL PROPERTY DIGGLER_INCLUDE_DIRECTORIES "")
function(diggler_include_directories)
  get_property(INCDIRS GLOBAL PROPERTY DIGGLER_INCLUDE_DIRECTORIES)
  list(APPEND INCDIRS ${ARGV})
  set_property(GLOBAL PROPERTY DIGGLER_INCLUDE_DIRECTORIES "${INCDIRS}")
endfunction(diggler_include_directories)

# Link libraries
set_property(GLOBAL PROPERTY DIGGLER_LINK_LIBRARIES "")
function(diggler_link_libraries)
  get_property(LIBS GLOBAL PROPERTY DIGGLER_LINK_LIBRARIES)
  list(APPEND LIBS ${ARGV})
  set_property(GLOBAL PROPERTY DIGGLER_LINK_LIBRARIES "${LIBS}")
endfunction(diggler_link_libraries)

set(DIGGLER_USED_ROOT_CMAKELISTS TRUE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

### Create target and apply parameters
# Include directories
get_property(INCDIRS GLOBAL PROPERTY DIGGLER_INCLUDE_DIRECTORIES)
include_directories(BEFORE ${INCDIRS})

# Sources
get_property(SRCS GLOBAL PROPERTY DIGGLER_SOURCE_FILES)
add_executable(diggler ${SRCS})

# Definitions
get_property(DEFS GLOBAL PROPERTY DIGGLER_DEFINITIONS)
target_compile_definitions(diggler PUBLIC ${DEFS})

# Link libraries
get_property(LIBS GLOBAL PROPERTY DIGGLER_LINK_LIBRARIES)
target_link_libraries(diggler
  ${LIBS}
  ${ETC_LIBRARIES}
  ${MEIOSE_LIBRARY}
  ${LUA_LIBRARY}
  ${LZFX_LIBRARIES}
  ${ENET_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${LIBSODIUM_LIBRARIES}
  ${SQLITE3_LIBRARIES}
  ${OPENAL_LIBRARY}
  pthread
)

if(MINGW)
  target_link_libraries(diggler
    stdc++
    ws2_32
    openal.dll
    winmm
  )
endif()

if(NOT WIN32 AND NOT MINGW)
  add_custom_command(TARGET diggler PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${PATH_TO_TOPDIR}/assets"
    "${PROJECT_BINARY_DIR}/assets")

  add_custom_command(TARGET diggler PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/scripting/lua/api"
    "${PROJECT_BINARY_DIR}/assets/lua/ffi")
endif()

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
