set(MEIOSE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/meiose")

set(MEIOSE_INCLUDE_DIR "${MEIOSE_ROOT_DIR}/include/")
if(NOT EXISTS "${MEIOSE_INCLUDE_DIR}")
  unset(MEIOSE_INCLUDE_DIR)
endif()

add_subdirectory("${MEIOSE_ROOT_DIR}")
target_include_directories(meiose-static PRIVATE "${MPACK_INCLUDE_DIR}")

set(MEIOSE_LIBRARY "meiose-static")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(meiose
  FOUND_VAR MEIOSE_FOUND
  REQUIRED_VARS MEIOSE_INCLUDE_DIR MEIOSE_LIBRARY)

mark_as_advanced(MEIOSE_INCLUDE_DIR MEIOSE_LIBRARY)
