cmake_minimum_required(VERSION 3.5.1)
project(nodeEditorSample)

set(CMAKE_CXX_STANDARD 14)

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

find_package(TinyXML2 REQUIRED)
if (NOT TinyXML2_FOUND)
    message(ERROR " TinyXML2 not found!")
endif (NOT TinyXML2_FOUND)
INCLUDE_DIRECTORIES(${TinyXML2_INCLUDE_DIRS})

add_executable(nodeEditor sample/main.cpp sample/SampleExtension.cpp sample/SampleExtension.h sample/SampleEditorExtension.cpp sample/SampleEditorExtension.h)

TARGET_LINK_LIBRARIES(nodeEditor ImGui nodeGraph GLEW::GLEW ${OPENGL_LIBRARIES} ${GLFW_LIBRARY} ${TinyXML2_LIBRARIES})
