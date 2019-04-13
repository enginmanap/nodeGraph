cmake_minimum_required (VERSION 3.5.1)

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
        )

add_library(nodeGraph ${nodeGraph_SOURCES} ${nodeGraph_HEADERS})

if((NOT NODEGRAPH_AS_LIBRARY) OR (NOT ("${NODEGRAPH_AS_LIBRARY}" STREQUAL "True")))
    include(sample/CMakeLists.txt)
endif()