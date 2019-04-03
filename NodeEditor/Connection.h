//
// Created by engin on 25.03.2019.
//

#ifndef NODEGRAPH_CONNECTION_H
#define NODEGRAPH_CONNECTION_H

#include <vector>
#include <string>
#include <iostream>
#include "../imgui/imgui.h"
#include "Common.h"

class Node;//To avoid circular dependency

const float NODE_SLOT_RADIUS = 5.0f;

enum ConnectionType {
    ConnectionType_Color,
    ConnectionType_Vec3,
    ConnectionType_Vec4,
    ConnectionType_Float,
    ConnectionType_Int,
};

struct ConnectionDesc {
    std::string name;
    ConnectionType type;

    std::string getTypeString() {
        switch (type) {
            case ConnectionType::ConnectionType_Color: return "Color";
            case ConnectionType::ConnectionType_Vec3: return "Vector3";
            case ConnectionType::ConnectionType_Vec4: return "Vector4";
            case ConnectionType::ConnectionType_Float: return "Float";
            case ConnectionType::ConnectionType_Int: return "Integer";
            default: {
                std::cerr << "Missing type for Connection. Exiting" << std::endl;
                std::exit(-1);
            }
        }
    }
};

class Connection {
public:
    enum class Types {INPUT, OUTPUT};
private:
    Node* parent;
    ImVec2 pos = {0,0};
    ConnectionDesc desc;
    Types type;

    struct Connection *input = nullptr;
    std::vector<Connection *> output;
public:
    explicit Connection(Node* parent, ConnectionDesc desc, Connection::Types type) :
    parent(parent), desc(desc), type(type) {}

    bool isHovered(ImVec2 offset);
    Node* getInputNode() {
        if(input != nullptr) {
            return input->parent;
        }
        return nullptr;
    }

    Connection *getInput() const {
        return input;
    }

    void setInputConnection(Connection* input ) {
        this->input = input;
    };

    ImVec2 getPosition() { return pos;}
    void display(ImDrawList *drawList, const ImVec2 node_rect_min, ImVec2 &offset, ImVec2 &textSize);
    ImVec2 getTextSize() {
        return ImGui::CalcTextSize(desc.name.c_str());
    }
    void setPosition(float TitleSizeY, float& textSizeY, float xPosition);

    Types getType() const {
        return type;
    }

    ConnectionType getDataType() const {
        return desc.type;
    }

};

#endif //NODEGRAPH_CONNECTION_H
