cmake_minimum_required(VERSION 3.12)
project(nodeEditorSample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

INCLUDE(FindPkgConfig)

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIRS})

if (NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif (NOT OPENGL_FOUND)

add_executable(nodeEditor sample/main.cpp sample/SampleExtension.cpp sample/SampleExtension.h sample/SampleEditorExtension.cpp sample/SampleEditorExtension.h)

TARGET_LINK_LIBRARIES(nodeEditor ImGui nodeGraph ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARY})
