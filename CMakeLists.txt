cmake_minimum_required (VERSION 3.5.1)
project(NodeGraph)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Cmake)
set(PROJECT_VERSION 0.6)

IF (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -mconsole -Wall -Wextra -Wnon-virtual-dtor -pedantic")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wnon-virtual-dtor -pedantic")
ENDIF()

if((NOT NODEGRAPH_AS_LIBRARY) OR (NOT ("${NODEGRAPH_AS_LIBRARY}" STREQUAL "True")))
    include(libs/CmakeLists.txt)
endif()

set(nodeGraph_SOURCES
        "src/Connection.cpp"
        "src/Connection.h"
        "src/Node.cpp"
        "src/NodeGraph.cpp"

        )
set(nodeGraph_HEADERS
        "src/Common.h"
        "src/Node.h"
        "src/NodeGraph.h"
        "src/NodeExtension.h"
        "src/EditorExtension.h"
        src/NodeType.cpp src/NodeType.h)

INCLUDE(FindPkgConfig)


find_package(TinyXML2 REQUIRED)
if (NOT TinyXML2_FOUND)
    message(ERROR " TinyXML2 not found!")
else()
    message(ERROR " library folder is ${TinyXML2_LIBRARIES}")
    message(ERROR ${TinyXML2_LIBRARIES})
    message(ERROR " library folder is ${TinyXML2_LIBRARIES}")
endif (NOT TinyXML2_FOUND)
INCLUDE_DIRECTORIES(${TinyXML2_INCLUDE_DIRS})

add_library(nodeGraph ${nodeGraph_SOURCES} ${nodeGraph_HEADERS})
TARGET_LINK_LIBRARIES(nodeGraph ${TinyXML2_LIBRARIES})

if((NOT NODEGRAPH_AS_LIBRARY) OR (NOT ("${NODEGRAPH_AS_LIBRARY}" STREQUAL "True")))
    include(sample/CMakeLists.txt)
endif()