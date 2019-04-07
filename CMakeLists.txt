cmake_minimum_required (VERSION 3.5.1)

INCLUDE(libs/CmakeLists.txt)

set(nodeGraph_SOURCES
        "src/Connection.cpp"
        "src/Connection.h"
        "sample/main.cpp"
        "src/Node.cpp"
        "src/NodeGraph.cpp"

        )
set(nodeGraph_HEADERS
        "src/Common.h"
        "src/Node.h"
        "src/NodeGraph.h"
        "src/NodeExtension.h"
        "src/EditorExtension.h"
        )

add_library(nodeGraph ${nodeGraph_SOURCES} ${nodeGraph_HEADERS})

include(sample/CMakeLists.txt)