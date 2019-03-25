//
// Created by engin on 25.03.2019.
//

#ifndef SIL_NODETEST_DRAGNODE_H
#define SIL_NODETEST_DRAGNODE_H


#include "../imgui/imgui.h"
#include "Connection.h"

enum DragState {
    DragState_Default,
    DragState_Hover,
    DragState_BeginDrag,
    DragState_Draging,
    DragState_Connect,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DragNode {
    ImVec2 pos;
    Connection *con;
};
#endif //SIL_NODETEST_DRAGNODE_H
