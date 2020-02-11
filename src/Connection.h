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
    bool combinedInput;//default false by constructor

    std::vector<Connection *> inputList;
    std::vector<Connection *> outputList;
public:
    Connection(Node* parent, ConnectionDesc desc, Connection::Directions direction) :
    Connection(parent, desc, direction, false) {}

    Connection(Node* parent, ConnectionDesc desc, Connection::Directions direction, bool combinedInput) :
            parent(parent), desc(desc), direction(direction), combinedInput(combinedInput) {}

    ~Connection();

    bool isHovered(ImVec2 offset);

    std::vector<Node*> getInputNodes() {
        std::vector<Node*> nodes;
        for(auto connection:inputList) {
            nodes.push_back(connection->parent);
        }
        return nodes;
    }

    std::vector<Connection *> getInputConnections() const {
        return inputList;
    }

    void addInputConnection(Connection* input ) {
        if (!combinedInput) {
            this->inputList.clear();
        }
        this->inputList.push_back(input);
        input->outputList.push_back(this);
    }

    void removeInputConnection(Connection* input ) {
        for(std::vector<Connection*>::iterator it= this->inputList.begin(); it != this->inputList.end(); it++) {
            if((*it) == input) {
                this->inputList.erase(it);
                return;
            }
        }
    }
/*
    Connection * getInputConnection() const {
        return this->input;
    }*/

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
