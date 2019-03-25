//
// Created by engin on 25.03.2019.
//

#ifndef SIL_NODETEST_NODEGRAPH_H
#define SIL_NODETEST_NODEGRAPH_H


#include "Node.h"

class NodeGraph {
    std::vector<Node *> nodes;
public:
    void display();
    // TODO: Ugly fix: me
    Node *findNodeByCon(Connection *findCon);
    void renderLines(ImDrawList *drawList, ImVec2 offset);

    Connection *getHoverCon(ImVec2 offset, ImVec2 *pos);

    void updateDragging(ImVec2 offset);

};


#endif //SIL_NODETEST_NODEGRAPH_H
