//
// Created by engin on 25.03.2019.
//

#include "Connection.h"
#include "Node.h"

Connection *getHoverCon(ImVec2 offset, ImVec2 *pos) {
    for (Node *node : s_nodes) {
        ImVec2 nodePos = node->pos + offset;

        for (Connection *con : node->inputConnections) {
            if (con->isHovered(nodePos)) {
                *pos = nodePos + con->pos;
                return con;
            }
        }

        for (Connection *con : node->outputConnections) {
            if (con->isHovered(nodePos)) {
                *pos = nodePos + con->pos;
                return con;
            }
        }
    }

    s_dragNode.con = 0;
    return 0;
}

void setupConnections(std::vector<Connection *> &connections, const ConnectionDesc *connectionDescs) {
    for (int i = 0; i < MAX_CONNECTION_COUNT; ++i) {
        const ConnectionDesc &desc = connectionDescs[i];

        if (!desc.name)
            break;

        Connection *con = new Connection;
        con->desc = desc;

        connections.push_back(con);
    }
}

bool Connection::isHovered(ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImVec2 conPos = offset + this->pos;

    float xd = mousePos.x - conPos.x;
    float yd = mousePos.y - conPos.y;

    return ((xd * xd) + (yd * yd)) < (NODE_SLOT_RADIUS * NODE_SLOT_RADIUS);
}