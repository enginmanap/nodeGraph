//
// Created by engin on 25.03.2019.
//

#ifndef NODEGRAPH_CONNECTION_H
#define NODEGRAPH_CONNECTION_H

#include <vector>
#include <string>
#include <iostream>
#include "ImGui/imgui.h"
#include "Common.h"

class Node;//To avoid circular dependency

const float NODE_SLOT_RADIUS = 5.0f;

struct ConnectionDesc {
    std::string name;
    std::string type;

};

class Connection {
public:
    enum class Directions {INPUT, OUTPUT};
private:
    Node* parent;
    ImVec2 pos = {0,0};
    ConnectionDesc desc;
    Directions direction;

    struct Connection *input = nullptr;
    std::vector<Connection *> output;
public:
    Connection(Node* parent, ConnectionDesc desc, Connection::Directions direction) :
    parent(parent), desc(desc), direction(direction) {}

    ~Connection();

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
        input->output.push_back(this);
    }

    Connection * getInputConnection() const {
        return this->input;
    }

    ImVec2 getPosition() { return pos;}
    void display(ImDrawList *drawList, const ImVec2 node_rect_min, ImVec2 &offset, ImVec2 &textSize);
    void displayDataTooltip();

    ImVec2 getTextSize() {
        return ImGui::CalcTextSize(desc.name.c_str());
    }
    void setPosition(float TitleSizeY, float& textSizeY, float xPosition);

    Directions getDirection() const {
        return direction;
    }

    const std::string &getDataType() const {
        return desc.type;
    }

    const std::string &getName() const {
        return desc.name;
    }

    void clearConnections();

    std::vector<Node*> getConnectedNodes() const;

    Node* getParent() {
        return parent;
    }
};

#endif //NODEGRAPH_CONNECTION_H
