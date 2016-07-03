set(MSGPACK_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/msgpack-c")

#set(MSGPACK_CXX11 ON)
#add_subdirectory("${MSGPACK_ROOT_DIR}")

set(MSGPACK_INCLUDE_DIR "${MSGPACK_ROOT_DIR}/include")
if(NOT EXISTS "${MSGPACK_INCLUDE_DIR}")
  set(MSGPACK_INCLUDE_DIR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(msgpack
  REQUIRED_VARS MSGPACK_INCLUDE_DIR)

mark_as_advanced(MSGPACK_INCLUDE_DIR)
