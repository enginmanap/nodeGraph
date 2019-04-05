cmake_minimum_required(VERSION 3.12)
project(nodegraph)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions(-DMAX_CONNECTION_COUNT=32)


set(IMGUI_SOURCES
        "ImGui/imgui.cpp"
        "ImGui/imgui_demo.cpp"
        "ImGui/imgui_draw.cpp"
        "ImGui/imgui_widgets.cpp"
        "ImGui/imgui_impl_glfw.cpp"
        "ImGui/imgui_impl_opengl3.cpp"
        )

set(IMGUI_HEADERS
        "ImGui/imconfig.h"
        "ImGui/imgui.h"
        "ImGui/imgui_internal.h"
        "ImGui/imstb_rectpack.h"
        "ImGui/imstb_textedit.h"
        "ImGui/imstb_truetype.h"
        "ImGui/imgui_impl_glfw.h"
        "ImGui/imgui_impl_opengl3.h"
        )


add_library(ImGui ${IMGUI_SOURCES} ${IMGUI_HEADERS})

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

add_executable(nodegraph NodeEditor/Common.h NodeEditor/Connection.cpp NodeEditor/Connection.h main.cpp NodeEditor/Node.cpp NodeEditor/Node.h NodeEditor/NodeGraph.cpp NodeEditor/NodeGraph.h)

TARGET_LINK_LIBRARIES(nodegraph ImGui ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARY})
