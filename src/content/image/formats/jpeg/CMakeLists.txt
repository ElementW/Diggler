set(DIGGLER_IMAGE_JPEG_LOADER "stbi" CACHE STRING
  "JPEG loader to use. One of 'stbi'")

if ("${DIGGLER_IMAGE_JPEG_LOADER}" STREQUAL "stbi")
  set(DIGGLER_IMAGE_JPEG_LOADER_SUPPORTED TRUE)
  diggler_link_libraries("stb_image")
endif()

if (NOT DIGGLER_IMAGE_JPEG_LOADER_SUPPORTED)
  message(SEND_ERROR "JPEG loader \"${DIGGLER_IMAGE_JPEG_LOADER}\" is unsupported")
endif()

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/JPEGLoader.cpp
  ${CSD}/JPEGLoader.${DIGGLER_IMAGE_JPEG_LOADER}.cpp
)
