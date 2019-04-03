//
// Created by engin on 25.03.2019.
//

#ifndef NODEGRAPH_NODEGRAPH_H
#define NODEGRAPH_NODEGRAPH_H


#include "Node.h"

class NodeGraph {
    enum class DisplayStates { NODE_GRAPH,
            MENU_REQUEST, MENU_SHOWN,
            RENAME_NODE_REQUEST, RENAME_NODE,
            ADD_CONNECTION_REQUEST, ADD_CONNECTION, };

    DisplayStates state = DisplayStates::NODE_GRAPH;
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
    Node* selectedNode = nullptr;
    Connection* hoveredConnection = nullptr;
    Connection::Directions connectionRequestType = Connection::Directions::INPUT;

    char nodeName[128] = {0};
    char connectionName[128] = {0};
    DragState dragState = DragState_Default;
    DragNode dragNode;

    void drawHermite(ImDrawList *drawList, ImVec2 p1, ImVec2 p2, int STEPS);

public:
    void display();
    Node *findNodeByCon(Connection *findCon);
    void renderLines(ImDrawList *drawList, ImVec2 offset);

    Connection *getHoverCon(ImVec2 offset, ImVec2 *pos);

    void updateDragging(ImVec2 offset);

    void drawContextMenu(Node *selectedNode, const ImVec2 &offset);
    void drawRenameMenu(Node *selectedNode);
    void drawAddConnectionMenu(Node *pNode);
};


#endif //NODEGRAPH_NODEGRAPH_H
