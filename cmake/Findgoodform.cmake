set(GOODFORM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/goodform")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D'htonll(x)=((1==htonl(1))?(x):((uint64_t)htonl((x)&0xFFFFFFFF)<<32)|htonl((x)>>32))' -D'ntohll(x)=((1==ntohl(1))?(x):((uint64_t)ntohl((x)&0xFFFFFFFF)<<32)|ntohl((x)>>32))'")

add_subdirectory("${GOODFORM_ROOT_DIR}")

set(GOODFORM_INCLUDE_DIR "${GOODFORM_ROOT_DIR}/include")
if(NOT EXISTS "${GOODFORM_INCLUDE_DIR}")
  set(GOODFORM_INCLUDE_DIR)
endif()

set(GOODFORM_LIBRARY goodform-static)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(goodform
  REQUIRED_VARS GOODFORM_INCLUDE_DIR GOODFORM_LIBRARY)

mark_as_advanced(GOODFORM_INCLUDE_DIR GOODFORM_LIBRARY)
