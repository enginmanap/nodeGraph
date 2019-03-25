//
// Created by engin on 25.03.2019.
//

#ifndef SIL_NODETEST_CONNECTION_H
#define SIL_NODETEST_CONNECTION_H

#include <vector>

#include "../imgui/imgui.h"
#include "Common.h"

class Node;//To avoid circular dependency

const float NODE_SLOT_RADIUS = 5.0f;

enum ConnectionType {
    ConnectionType_Color,
    ConnectionType_Vec3,
    ConnectionType_Float,
    ConnectionType_Int,
};

struct ConnectionDesc {
    const char *name;
    ConnectionType type;
};

struct Connection {
    Node* parent;
    ImVec2 pos;
    ConnectionDesc desc;

    inline Connection() {
        pos.x = pos.y = 0.0f;
        input = 0;
    }

    union {
        float v3[3];
        float v;
        int i;
    };

    Connection(Node* parent) : parent(parent) {}
    struct Connection *input = nullptr;
    std::vector<Connection *> output;

    bool isHovered(ImVec2 offset);

    Node* getInputNode();

};

#endif //SIL_NODETEST_CONNECTION_H
