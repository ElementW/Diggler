add_subdirectory("flif")
add_subdirectory("jpeg")
add_subdirectory("png")

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/ImageLoader.cpp
  ${CSD}/STBImageLoader.cpp
)
