set(DIGGLER_IMAGE_PNG_LOADER "stbi" CACHE STRING
  "PNG loader to use. One of 'stbi'")

if ("${DIGGLER_IMAGE_PNG_LOADER}" STREQUAL "stbi")
  set(DIGGLER_IMAGE_PNG_LOADER_SUPPORTED TRUE)
  diggler_link_libraries("stb_image")
endif()

if (NOT DIGGLER_IMAGE_PNG_LOADER_SUPPORTED)
  message(SEND_ERROR "PNG loader \"${DIGGLER_IMAGE_PNG_LOADER}\" is unsupported")
endif()

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/PNGLoader.cpp
  ${CSD}/PNGLoader.${DIGGLER_IMAGE_PNG_LOADER}.cpp
)
