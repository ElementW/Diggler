add_subdirectory("formats")

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/ImageLoader.cpp
)
