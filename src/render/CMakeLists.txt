set(DIGGLER_RENDERER "gl" CACHE STRING "Graphics renderer to compile and use. Available: gl.")

if(DIGGLER_RENDERER STREQUAL "gl")
  add_subdirectory("gl")
else()
  message(FATAL_ERROR "Unknown renderer")
endif()
