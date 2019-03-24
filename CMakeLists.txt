cmake_minimum_required(VERSION 3.13)
project(sil_nodetest)

set(CMAKE_CXX_STANDARD 14)

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

find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIRS})

if (NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif (NOT OPENGL_FOUND)

add_executable(sil_nodetest NodeEditor/Common.h NodeEditor/Connection.cpp NodeEditor/Connection.h main.cpp NodeEditor/Node.cpp NodeEditor/Node.h NodeEditor/DragNode.cpp NodeEditor/DragNode.h)

TARGET_LINK_LIBRARIES(sil_nodetest ImGui  ${GLEW_LIBRARIES}  ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
