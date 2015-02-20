cmake_minimum_required(VERSION 2.8)

project(diggler)

### Find path to source dir, relative if possible
file(RELATIVE_PATH REL_BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
string(FIND ${REL_BUILD_PATH} ".." RBP_2DOTPOS)
# If we're going up, use absolute path
if(${RBP_2DOTPOS} EQUAL 0)
	set(PATH_TO_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
else()
	file(RELATIVE_PATH PATH_TO_BUILD_DIR ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
endif()
message(">> Path to build dir: ${PATH_TO_BUILD_DIR}")

### Compiler & build options
# Disable narrow convert warnings and disable deprecated GLM degrees usage
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wno-narrowing -ffast-math -DGLM_FORCE_RADIANS")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Define __FILENAME__ as the currently compiled filename
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
#" (fix buggy syntax highlighting on KDevelop/Kate)

if(NOT CMAKE_BUILD_TYPE) # Defaults to release
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(USE_GCC_LTO FALSE CACHE BOOL "Use GCC LTOs")
endif()

message(">> Using ${CMAKE_BUILD_TYPE} build configuration")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG -ggdb -Wall -Wuninitialized")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
if(USE_GCC_LTO)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
endif()

### Packages
add_subdirectory(enet)
add_subdirectory(lzfx)
find_package(GLM)
find_package(OpenAL)
find_package(OpenGL)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${ENET_INCLUDE_DIR})

add_executable(diggler Audio.cpp ui/Element.cpp ui/Manager.cpp UITestState.cpp ChunkChangeHelper.cpp Mutex.cpp PlayerList.cpp network/NetHelper.cpp network/Network.cpp Skybox.cpp Frustum.cpp CaveGenerator.cpp MessageState.cpp GameState.cpp network/Network.cpp Server.cpp Chatbox.cpp GlUtils.cpp ui/Button.cpp KeyBindings.cpp GameWindow.cpp GLFWHandler.cpp Player.cpp Clouds.cpp Game.cpp ProgramManager.cpp LocalPlayer.cpp AABB.cpp SoundBuffer.cpp Sound.cpp stb_vorbis.c Camera.cpp EscMenu.cpp ui/Text.cpp Font.cpp AtlasCreator.cpp Blocks.cpp GlobalProperties.cpp Superchunk.cpp Chunk.cpp FBO.cpp Texture.cpp stb_image.c Program.cpp VBO.cpp Shader.cpp Platform.cpp main.cpp)

target_link_libraries(diggler GLEW GL enet lzfx ${GLFW_STATIC_LIBRARIES} ${OPENAL_LIBRARY})

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${PATH_TO_BUILD_DIR}/assets" "${PROJECT_BINARY_DIR}/assets")

install(TARGETS diggler RUNTIME DESTINATION bin)