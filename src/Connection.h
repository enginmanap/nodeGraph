//
// Created by engin on 25.03.2019.
//

#ifndef NODEGRAPH_CONNECTION_H
#define NODEGRAPH_CONNECTION_H

#include <vector>
#include <string>
#include <iostream>
#include <tinyxml2.h>
#include "ImGui/imgui.h"
#include "Common.h"

class Node;//To avoid circular dependency

const float NODE_SLOT_RADIUS = 5.0f;

struct ConnectionDesc {
    std::string name;
    std::string type;

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
        tinyxml2::XMLElement * connectionDescElement = document.NewElement("ConnectionDesc");
        parentElement->InsertEndChild(connectionDescElement);

        tinyxml2::XMLElement * nameElement = document.NewElement("Name");
        nameElement->SetText(name.c_str());
        connectionDescElement->InsertEndChild(nameElement);

        tinyxml2::XMLElement * typeElement = document.NewElement("Type");
        typeElement->SetText(type.c_str());
        connectionDescElement->InsertEndChild(typeElement);
    }

    static bool deserialize(const std::string& fileName, tinyxml2::XMLElement *connectionDescElement, ConnectionDesc& elementToFill) {

        tinyxml2::XMLElement *nameElement = connectionDescElement->FirstChildElement("Name");
        if (nameElement == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Name of ConnectionDesc is not found!" << std::endl;
            return false;
        }

        if (nameElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Name of ConnectionDesc has no text!" << std::endl;
            return false;
        }
        elementToFill.name = nameElement->GetText();

        tinyxml2::XMLElement *typeElement = connectionDescElement->FirstChildElement("Type");
        if (typeElement == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Type of ConnectionDesc is not found!" << std::endl;
            return false;
        }

        if (typeElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Type of ConnectionDesc has no text!" << std::endl;
            return false;
        }
        elementToFill.type = typeElement->GetText();
        return true;
    }
    };

struct LateDeserializeInformation {
    uint32_t nodeID;
    uint32_t connectionID;
    std::string connectionName;
};

class Connection {
public:
    enum class Directions {INPUT, OUTPUT};
private:
    uint32_t id;//This id should be unique per node, not global. used for serialize/deserialize.
    Node* parent;
    ImVec2 pos = {0,0};
    ConnectionDesc desc;
    Directions direction;
    bool combinedInput;//default false by constructor

    std::vector<Connection *> inputList;
    std::vector<Connection *> outputList;
public:
    Connection(Node* parent, uint32_t id, ConnectionDesc desc, Connection::Directions direction) :
    Connection(parent, id, desc, direction, false) {}

    Connection(Node* parent, uint32_t id, ConnectionDesc desc, Connection::Directions direction, bool combinedInput) :
            id(id), parent(parent), desc(desc), direction(direction), combinedInput(combinedInput) {}

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

    uint32_t getId() const {
        return id;
    }

    void display(ImDrawList *drawList, const ImVec2 node_rect_min, ImVec2 &offset, ImVec2 &textSize);
    void displayDataTooltip() const;

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

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement);
    static Connection* deserialize(const std::string &fileName,
                                   tinyxml2::XMLElement *connectionElement, Node* parentNode,
                                   std::vector<LateDeserializeInformation>& inputs,
                                   std::vector<LateDeserializeInformation>& outputs);

    void lateSerialize(const std::vector<LateDeserializeInformation> &lateDeserializeList, std::vector<Node *> allNodes);
};

#endif //NODEGRAPH_CONNECTION_H
