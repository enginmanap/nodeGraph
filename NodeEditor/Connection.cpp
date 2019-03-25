//
// Created by engin on 25.03.2019.
//

#include "Connection.h"
#include "Node.h"

bool Connection::isHovered(ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImVec2 conPos = offset + this->pos;

    float xd = mousePos.x - conPos.x;
    float yd = mousePos.y - conPos.y;

    return ((xd * xd) + (yd * yd)) < (NODE_SLOT_RADIUS * NODE_SLOT_RADIUS);
}

Node* Connection::getInputNode() {
    if(input != nullptr) {
        return input->parent;
    }
    return nullptr;
}

