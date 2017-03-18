add_subdirectory("audio")
add_subdirectory("font")
add_subdirectory("image")
add_subdirectory("shader")
add_subdirectory("texture")

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/AssetManager.cpp
  ${CSD}/ModManager.cpp
  ${CSD}/Registry.cpp
)
