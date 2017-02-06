set(DIGGLER_ENABLE_GL_RENDERER TRUE CACHE BOOL "Enable OpenGL (ES) renderer")
set(DIGGLER_ENABLE_VK_RENDERER TRUE CACHE BOOL "Enable Vulkan renderer")

if(DIGGLER_ENABLE_GL_RENDERER)
  add_subdirectory("gl")
endif()

if(DIGGLER_ENABLE_VK_RENDERER)
  add_subdirectory("vk")
endif()

set(SRCS
  ${SRCS}
  PARENT_SCOPE
)
