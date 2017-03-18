set(DIGGLER_ENABLE_GL_RENDERER TRUE CACHE BOOL "Enable OpenGL (ES) renderer")

set(DIGGLER_GL_LOADER "libepoxy" CACHE STRING
  "OpenGL loader to use. One of 'libepoxy', 'glad', 'glbinding', 'glew'")

if (DIGGLER_ENABLE_GL_RENDERER)
  if ("${DIGGLER_GL_LOADER}" STREQUAL "libepoxy")
    pkg_search_module(EPOXY REQUIRED epoxy)
    set(DIGGLER_GL_USE_LIBEPOXY TRUE)
    set(DIGGLER_GL_LOADER_SUPPORTED TRUE)
    diggler_add_definition("DIGGLER_GL_USE_LIBEPOXY")
    diggler_include_directories(${EPOXY_INCLUDE_DIRS})
    diggler_link_libraries(${EPOXY_LIBRARIES})
  endif()
  if ("${DIGGLER_GL_LOADER}" STREQUAL "glad")
    set(DIGGLER_GL_USE_GLAD TRUE)
    set(DIGGLER_GL_LOADER_SUPPORTED TRUE)
    diggler_add_definition("DIGGLER_GL_USE_GLAD")
    diggler_add_sources("${CMAKE_CURRENT_SOURCE_DIR}/../glad.c")
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
      diggler_link_libraries(dl)
    endif()
  endif()
  if ("${DIGGLER_GL_LOADER}" STREQUAL "glbinding")
    find_package(glbinding REQUIRED)
    set(DIGGLER_GL_USE_GLAD TRUE)
    set(DIGGLER_GL_LOADER_SUPPORTED TRUE)
    diggler_add_definition("DIGGLER_GL_USE_GLBINDING")
    diggler_include_directories(${glbinding_INCLUDE_DIRS})
    diggler_link_libraries(glbinding)
  endif()
  if ("${DIGGLER_GL_LOADER}" STREQUAL "glew")
    pkg_search_module(GLEW REQUIRED glew)
    set(DIGGLER_GL_USE_GLEW TRUE)
    set(DIGGLER_GL_LOADER_SUPPORTED TRUE)
    diggler_add_definition("DIGGLER_GL_USE_GLEW")
    diggler_include_directories(${GLEW_INCLUDE_DIRS})
    diggler_link_libraries(${GLEW_LIBRARIES})
  endif()

  if (NOT DIGGLER_GL_LOADER_SUPPORTED)
    message(SEND_ERROR "OpenGL loader \"${DIGGLER_GL_LOADER}\" is unsupported")
  endif()

  set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
  diggler_add_sources(
    ${CSD}/Debug.cpp
    ${CSD}/DelegateGL.cpp
    ${CSD}/FBO.cpp
    ${CSD}/FeatureSupport.cpp
    ${CSD}/FontRenderer.cpp
    ${CSD}/OpenGL.cpp
    ${CSD}/OpenGL.${DIGGLER_GL_LOADER}.cpp
    ${CSD}/ParticlesRenderer.cpp
    ${CSD}/Program.cpp
    ${CSD}/ProgramManager.cpp
    ${CSD}/Renderer.cpp
    ${CSD}/Shader.cpp
    ${CSD}/Texture.cpp
    ${CSD}/TextureManager.cpp
    ${CSD}/Util.cpp
    ${CSD}/VBO.cpp
    ${CSD}/WorldRenderer.cpp
  )
endif()
