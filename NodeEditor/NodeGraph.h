//
// Created by engin on 25.03.2019.
//

#ifndef SIL_NODETEST_NODEGRAPH_H
#define SIL_NODETEST_NODEGRAPH_H


#include "Node.h"

class NodeGraph {
    uint32_t nextNodeID = 0;

    enum DragState {
        DragState_Default,
        DragState_Hover,
        DragState_BeginDrag,
        DragState_Draging,
        DragState_Connect,
    };

    struct DragNode {
        ImVec2 pos;
        Connection *con;
    };

    std::vector<Node *> nodes;

    DragState dragState = DragState_Default;
    DragNode dragNode;

    void drawHermite(ImDrawList *drawList, ImVec2 p1, ImVec2 p2, int STEPS);

public:
    void display();
    // TODO: Ugly fix: me
    Node *findNodeByCon(Connection *findCon);
    void renderLines(ImDrawList *drawList, ImVec2 offset);

    Connection *getHoverCon(ImVec2 offset, ImVec2 *pos);

    void updateDragging(ImVec2 offset);

    void DrawContextMenu();
};


#endif //SIL_NODETEST_NODEGRAPH_H
