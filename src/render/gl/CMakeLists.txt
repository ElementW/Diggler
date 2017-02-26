set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
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
  ${CSD}/Util.cpp
  ${CSD}/VBO.cpp
  ${CSD}/WorldRenderer.cpp
)
