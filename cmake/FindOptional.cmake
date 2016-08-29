set(OPTIONAL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/Optional")

set(OPTIONAL_INCLUDE_DIR "${OPTIONAL_ROOT_DIR}")
if(NOT EXISTS "${OPTIONAL_INCLUDE_DIR}")
  unset(OPTIONAL_INCLUDE_DIR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Optional
  REQUIRED_VARS OPTIONAL_INCLUDE_DIR)

mark_as_advanced(OPTIONAL_INCLUDE_DIR)
