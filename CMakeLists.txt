cmake_minimum_required(VERSION 2.6)

project(diggler)

add_subdirectory(enet)
add_subdirectory(lzfx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wno-narrowing -ffast-math -DGLM_FORCE_RADIANS")
# makes GCC STFU about narrow converts, and GLM STFU about degree angles
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
#set(USE_GCC_LTO FALSE CACHE BOOL "Use GCC LTOs")
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "MinSizeRel")
message(">> Using ${CMAKE_BUILD_TYPE} configuration")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG -ggdb -Wall -Wuninitialized")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
#if(USE_GCC_LTO)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
#endif()

find_package(OpenAL)
find_package(OpenGL)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${ENET_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
#"

add_executable(diggler Audio.cpp ui/Element.cpp ui/Manager.cpp UITestState.cpp ChunkChangeHelper.cpp Mutex.cpp PlayerList.cpp network/NetHelper.cpp network/Network.cpp Skybox.cpp Frustum.cpp CaveGenerator.cpp MessageState.cpp GameState.cpp network/Network.cpp Server.cpp Chatbox.cpp GlUtils.cpp ui/Button.cpp KeyBindings.cpp GameWindow.cpp GLFWHandler.cpp Player.cpp Clouds.cpp Game.cpp ProgramManager.cpp LocalPlayer.cpp AABB.cpp SoundBuffer.cpp Sound.cpp stb_vorbis.c Camera.cpp EscMenu.cpp ui/Text.cpp Font.cpp AtlasCreator.cpp Blocks.cpp GlobalProperties.cpp Superchunk.cpp Chunk.cpp FBO.cpp Texture.cpp stb_image.c Program.cpp VBO.cpp Shader.cpp Platform.cpp main.cpp)

target_link_libraries(diggler GLEW GL enet lzfx ${GLFW_STATIC_LIBRARIES} ${OPENAL_LIBRARY})

install(TARGETS diggler RUNTIME DESTINATION bin)