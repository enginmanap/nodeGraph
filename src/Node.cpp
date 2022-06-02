//
// Created by engin on 24.03.2019.
//

#include <sstream>
#include "Node.h"

Node::Node(uint32_t id, ImVec2 pos, const NodeType *nodeType) {
    initialize(id, pos, nodeType);
}

Node::Node(uint32_t id, ImVec2 pos, const std::vector<NodeType*> &allNodeTypes, const char *name, uint32_t &error) {
    for (size_t i = 0; i < allNodeTypes.size(); ++i) {
        if (!strcmp(allNodeTypes[i]->name.c_str(), name)) {
            initialize(id, pos, allNodeTypes[i]);
            error = 0;
            return;
        }
    }
    error = 1;
}

void Node::initialize(uint32_t id, const ImVec2 &pos, const NodeType *nodeType) {
    this->id = id;
    this->pos = pos;
    this->name = nodeType->name;
    this->displayName = this->name;
    this->editable = nodeType->editable;
    if(nodeType->nodeExtensionConstructor != nullptr) {
        this->nodeExtension = nodeType->nodeExtensionConstructor(nodeType);
    } else {
        if(!nodeType->extensionName.empty()) {
            std::cerr << "Node type has extension name but no constructor, no extension created!" << std::endl;
        }
    }
    this->combineInputs = nodeType->combineInputs;
    this->nextConnectionId = 0;

    setupConnections(inputConnections, nodeType->inputConnections, Connection::Directions::INPUT);
    setupConnections(outputConnections, nodeType->outputConnections, Connection::Directions::OUTPUT);
    calculateAndSetDrawInformation();

}

void Node::calculateAndSetDrawInformation() {
    // Calculate the size needed for the whole box

    ImVec2 titleSize = ImGui::CalcTextSize(displayName.c_str());

    titleSize.y *= 3;
    titleSize.x += 5;

    ImVec2 inputTextSize(0.0f, 0.0f);
    ImVec2 outputTextSize(0.0f, 0.0f);

    for (Connection *c : inputConnections) {
        ImVec2 textSize = c->getTextSize();
        inputTextSize.x = std::max<float>(textSize.x, inputTextSize.x);
        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;        // size between text entries
    }
    inputTextSize.x += 40.0f;

    // max text size + 40 pixels in between
    float xStart = inputTextSize.x;

    // Calculate for the outputs
    for (Connection *c : outputConnections) {
        ImVec2 textSize = c->getTextSize();
        inputTextSize.x = std::max<float>(xStart + textSize.x, inputTextSize.x);
        outputTextSize.y += textSize.y;
        outputTextSize.y += 4.0f;        // size between text entries
    }


    size.x = std::max(titleSize.x, inputTextSize.x);
    size.y = std::max(inputTextSize.y, outputTextSize.y) + titleSize.y;

    inputTextSize.y = 0.0f;

    for (Connection *c : inputConnections) {
        c->setPosition(titleSize.y, inputTextSize.y, 0);
    }

    // set the positions for the output nodes when we know where the place them
    outputTextSize.y = 0.0f;

    for (Connection *c : outputConnections) {
        c->setPosition(titleSize.y, outputTextSize.y, size.x);
    }

    // calculate the size of the node depending on nuber of connections
}

void Node::setupConnections(std::vector<Connection *> &connections, const std::vector<ConnectionDesc> &connectionDescs, Connection::Directions connectionType) {
    for (auto connectionDescription:connectionDescs) {

        if (connectionDescription.name.empty())
            break;

        Connection *con = new Connection(this, ++nextConnectionId, connectionDescription, connectionType, this->combineInputs);

        connections.push_back(con);
    }
}

void Node::display(ImDrawList *drawList, ImVec2 offset, bool dragNodeConnected, bool isThisNodeSelected) {
    uint32_t node_hovered_in_scene = -1;
    bool open_context_menu = false;

    ImGui::PushID(id);
    ImVec2 node_rect_min = offset + pos;

    // Display node contents first
    drawList->ChannelsSetCurrent(1); // Foreground

    // Draw title in center

    ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());

    ImVec2 pos = node_rect_min + NODE_WINDOW_PADDING;
    pos.x = node_rect_min.x + (size.x / 2) - textSize.x / 2;

    // Save the size of what we have emitted and weither any of the widgets are being used
    ImVec2 node_rect_max = node_rect_min + size;

    // Display node box
    drawList->ChannelsSetCurrent(0); // Background

    ImGui::SetCursorScreenPos(node_rect_min);
    ImGui::InvisibleButton("node", size);
    ImGui::SetItemAllowOverlap();
    if (ImGui::IsItemHovered()) {
        node_hovered_in_scene = id;
        open_context_menu |= ImGui::IsMouseClicked(1);
    }

    bool node_moving_active = false;

    if (ImGui::IsItemActive() && !dragNodeConnected) {
        node_moving_active = true;
    }

    ImU32 node_bg_color = node_hovered_in_scene == id ? ImColor(75, 75, 75) : ImColor(60, 60, 60);
    drawList->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);

    ImVec2 titleArea = node_rect_max;
    titleArea.y = node_rect_min.y + 30.0f;

    // Draw text bg area
    drawList->AddRectFilled(node_rect_min + ImVec2(1, 1), titleArea, ImColor(100, 0, 0), 4.0f);
    drawList->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);

    if(isThisNodeSelected) {
        ImU32 node_perimeter_color = ImColor(45, 45, 45);
        float borderSize = 2.0f;
        drawList->AddRect(node_rect_min - ImVec2(borderSize, borderSize), node_rect_max + ImVec2(borderSize, borderSize), node_perimeter_color, 4.0f, ImDrawCornerFlags_All, 2*borderSize);
    }


    offset = node_rect_min;
    offset.y += 40.0f;

    ImGui::SetCursorScreenPos(pos);
    //ImGui::BeginGroup(); // Lock horizontal position
    ImGui::Text("%s", displayName.c_str());

    for (Connection *con : inputConnections) {
        con->display(drawList, node_rect_min, offset, textSize);
    }

    offset = node_rect_min;
    offset.y += 40.0f;

    for (Connection *con : outputConnections) {
        con->display(drawList, node_rect_min, offset, textSize);
    }


    //for (int i = 0; i < node->outputConnections.size(); ++i)
    //	drawList->AddCircleFilled(offset + node->outputSlotPos(i), NODE_SLOT_RADIUS, ImColor(150,150,150,150));

    if (node_moving_active && ImGui::IsMouseDragging(0))
        this->pos = this->pos + ImGui::GetIO().MouseDelta;

    //ImGui::EndGroup();

    ImGui::PopID();
}

bool Node::hasConnection(Connection *connection) {
    for (Connection *con : inputConnections) {
        if (con == connection)
            return true;
    }

    for (Connection *con : outputConnections) {
        if (con == connection)
            return true;
    }
    return false;
}

Connection *Node::getHoverConnection(ImVec2 offset, ImVec2 *pos) {
    ImVec2 nodePos = this->pos + offset;

    for (Connection *con : this->inputConnections) {
        if (con->isHovered(nodePos)) {
            *pos = nodePos + con->getPosition();
            return con;
        }
    }

    for (Connection *con : this->outputConnections) {
        if (con->isHovered(nodePos)) {
            *pos = nodePos + con->getPosition();
            return con;
        }
    }

    return nullptr;
}

std::vector<LineDrawInformation> Node::getLinesToRender(const ImVec2& offset) {
    std::vector<LineDrawInformation> fromToPairs;
    ImVec2 tempPosition;//not used.
    for (Connection *con : this->inputConnections) {
        std::vector<Connection *> inputConnections = con->getInputConnections();
        if (inputConnections.empty()) {
            continue;
        }

        for(Connection * targetConnection:inputConnections) {
            if (!targetConnection) {
                continue;
            }
            LineDrawInformation drawInformation(targetConnection->getParent()->pos + targetConnection->getPosition() + offset,
                                                this->pos + con->getPosition() + offset,
                                                getHoverConnection(offset,&tempPosition),
                                                isHovered(offset)
            );
            fromToPairs.push_back(drawInformation);
        }
    }
    return fromToPairs;
}

bool Node::isHovered(ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    mousePos = mousePos - offset;
    //now check if mousepos is in the Node:
    if((mousePos.x > this->pos.x && mousePos.x < this->pos.x + this->size.x) && //x inside
       (mousePos.y > this->pos.y && mousePos.y < this->pos.y + this->size.y) //y inside
        ) {
        return true;
    }

    for(Connection* con:inputConnections) {
        if(con->isHovered(this->pos + offset)) {
            return true;
        }
    }
    for(Connection* con:outputConnections) {
        if(con->isHovered(this->pos + offset)) {
            return true;
        }
    }
    return false;
}

void Node::addInput(const ConnectionDesc &description) {
        if (description.name.empty()) {
            std::cerr << "Connection creation request failed because of emtpy name" << std::endl;
            return;
        }

        Connection *con = new Connection(this, ++nextConnectionId, description, Connection::Directions::INPUT, this->combineInputs);
        this->inputConnections.push_back(con);
        calculateAndSetDrawInformation();
}

void Node::removeInput(Connection *connection) {
    for(auto it = inputConnections.begin(); it != inputConnections.end(); it++) {
        if((*it) == connection) {
            inputConnections.erase(it);
            delete connection;
            calculateAndSetDrawInformation();
            break;
        }
    }
}

void Node::addOutput(const ConnectionDesc &description) {
    if (description.name.empty()) {
        std::cerr << "Connection creation request failed because of emtpy name" << std::endl;
        return;
    }

    Connection *con = new Connection(this, ++nextConnectionId, description, Connection::Directions::OUTPUT, this->combineInputs);
    this->outputConnections.push_back(con);
    calculateAndSetDrawInformation();
}

void Node::removeOutput(Connection *connection) {
    for(auto it = outputConnections.begin(); it != outputConnections.end(); it++) {
        if((*it) == connection) {
            outputConnections.erase(it);
            delete connection;
            calculateAndSetDrawInformation();
            break;
        }
    }
}
void Node::removeAllOutputs(){
    outputConnections.clear();
    calculateAndSetDrawInformation();
}

Node::~Node() {
    for(Connection* connection:inputConnections) {
        delete connection;
    }
    for(Connection* connection:outputConnections) {
        delete connection;
    }
}

std::set<Node*> Node::getOutputConnectedNodes() {
    std::set<Node*> nodes;
    for(Connection* connection:outputConnections) {
        std::vector<Node*> connectedNodes = connection->getConnectedNodes();
        nodes.insert(connectedNodes.begin(), connectedNodes.end());
    }
    return nodes;
}

void Node::serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {

    tinyxml2::XMLElement *nodeElement = document.NewElement("Node");
    parentElement->InsertEndChild(nodeElement);

    tinyxml2::XMLElement *idElement = document.NewElement("ID");
    idElement->SetText(std::to_string(id).c_str());
    nodeElement->InsertEndChild(idElement);

    tinyxml2::XMLElement *nameElement = document.NewElement("Name");
    nameElement->SetText(name.c_str());
    nodeElement->InsertEndChild(nameElement);

    tinyxml2::XMLElement *displayNameElement = document.NewElement("DisplayName");
    displayNameElement->SetText(displayName.c_str());
    nodeElement->InsertEndChild(displayNameElement);

    tinyxml2::XMLElement *editableElement = document.NewElement("Editable");
    editableElement->SetText(editable ? "True" : "False");
    nodeElement->InsertEndChild(editableElement);

    tinyxml2::XMLElement *combineInputsElement = document.NewElement("CombineInputs");
    combineInputsElement->SetText(combineInputs ? "True" : "False");
    nodeElement->InsertEndChild(combineInputsElement);


    tinyxml2::XMLElement *positionElement = document.NewElement("Position");
    nodeElement->InsertEndChild(positionElement);
    tinyxml2::XMLElement *posXElement = document.NewElement("X");
    posXElement->SetText(std::to_string(pos.x).c_str());
    positionElement->InsertEndChild(posXElement);
    tinyxml2::XMLElement *posYElement = document.NewElement("Y");
    posYElement->SetText(std::to_string(pos.y).c_str());
    positionElement->InsertEndChild(posYElement);

    tinyxml2::XMLElement *sizeElement = document.NewElement("Size");
    nodeElement->InsertEndChild(sizeElement);
    tinyxml2::XMLElement *sizeXElement = document.NewElement("X");
    sizeXElement->SetText(std::to_string(size.x).c_str());
    sizeElement->InsertEndChild(sizeXElement);
    tinyxml2::XMLElement *sizeYElement = document.NewElement("Y");
    sizeYElement->SetText(std::to_string(size.y).c_str());
    sizeElement->InsertEndChild(sizeYElement);

    tinyxml2::XMLElement *inputsElement = document.NewElement("Inputs");
    for (size_t i = 0; i < inputConnections.size(); ++i) {
        inputConnections[i]->serialize(document, inputsElement);
    }
    nodeElement->InsertEndChild(inputsElement);

    tinyxml2::XMLElement *outputsElement = document.NewElement("Outputs");
    for (size_t i = 0; i < outputConnections.size(); ++i) {
        outputConnections[i]->serialize(document, outputsElement);
    }
    nodeElement->InsertEndChild(outputsElement);
    if(nodeExtension != nullptr) {
        nodeExtension->serialize(document, nodeElement);
    }
}

Node *Node::deserialize(const std::string &fileName,
        tinyxml2::XMLElement *nodeElement,
        std::vector<NodeType*> possibleNodeTypes,
        std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeInputs,
        std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeOutputs) {

    tinyxml2::XMLElement* idElement =  nodeElement->FirstChildElement("ID");
    if (idElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": ID of Node is not found!" << std::endl;
        return nullptr;
    }

    if(idElement->GetText() == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": ID of Node has no text!" << std::endl;
        return nullptr;
    }
    uint32_t nodeID = std::stoul(idElement->GetText());

    tinyxml2::XMLElement* nameElement =  nodeElement->FirstChildElement("Name");
    if (nameElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Name of Node is not found!" << std::endl;
        return nullptr;
    }
    if(nameElement->GetText() == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Name of Node has no text!" << std::endl;
        return nullptr;
    }
    std::string nodeName = nameElement->GetText();

    ImVec2 position;
    tinyxml2::XMLElement* positionElement =  nodeElement->FirstChildElement("Position");
    if (positionElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Position of Node is not found!" << std::endl;
        return nullptr;
    } else {

        tinyxml2::XMLElement* posXElement =  positionElement->FirstChildElement("X");
        if (posXElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": X position of Node is not found!" << std::endl;
            return nullptr;
        }

        if(posXElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": X position of Node has no text!" << std::endl;
            return nullptr;
        }
        position.x = std::stof(posXElement->GetText());

        tinyxml2::XMLElement* posYElement =  positionElement->FirstChildElement("Y");
        if (posYElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Y position of Node is not found!" << std::endl;
            return nullptr;
        }

        if(posYElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Y position of Node has no text!" << std::endl;
            return nullptr;
        }
        position.y = std::stof(posYElement->GetText());
    }

    uint32_t error = 0;
    Node* newNode = new Node(nodeID, position, possibleNodeTypes, nodeName.c_str(), error);
    if(error != 0) {
        std::cerr << "Error loading XML "<< fileName << ": Node creation failed!" << std::endl;
        return nullptr;
    }

    tinyxml2::XMLElement* displayNameElement =  nodeElement->FirstChildElement("DisplayName");
    if (displayNameElement == nullptr || displayNameElement->GetText() == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": DisplayName of Node is not found! defaulting to node name." << std::endl;
    } else {
        newNode->setDisplayName(displayNameElement->GetText());
    }

    //now try to deserialize the connections
    tinyxml2::XMLElement* inputsElement =  nodeElement->FirstChildElement("Inputs");
    if (inputsElement == nullptr) {
        if(newNode->getInputConnections().size() != 0) {
            std::cerr << "Error loading XML " << fileName << ": Inputs node is not found! continue with defaults" << std::endl;
        }
    } else {
        newNode->inputConnections.clear();//there might be different kinds of connection modifications, just clear and load.
        tinyxml2::XMLElement* inputElement =  inputsElement->FirstChildElement("Connection");
        while(inputElement != nullptr) {
            std::vector<LateDeserializeInformation> inputLateDeserializeInfo;
            std::vector<LateDeserializeInformation> outputLateDeserializeInfo;
            Connection* connection = Connection::deserialize(fileName, inputElement, newNode, inputLateDeserializeInfo, outputLateDeserializeInfo);
            if(connection == nullptr) {
                return nullptr;
            }
            lateDeserializeInputs[connection] = inputLateDeserializeInfo;
            lateDeserializeOutputs[connection] = outputLateDeserializeInfo;
            newNode->inputConnections.emplace_back(connection);
            inputElement = inputElement->NextSiblingElement("Connection");
        }
    }

    tinyxml2::XMLElement* outputsElement =  nodeElement->FirstChildElement("Outputs");
    if (outputsElement == nullptr) {
        if(newNode->getOutputConnections().size() != 0) {
            std::cerr << "Error loading XML " << fileName << ": Outputs node is not found! continue with defaults" << std::endl;
        }
    } else {
        newNode->outputConnections.clear();//there might be different kinds of connection modifications, just clear and load.
        tinyxml2::XMLElement* outputElement =  outputsElement->FirstChildElement("Connection");
        while(outputElement != nullptr) {
            std::vector<LateDeserializeInformation> inputLateDeserializeInfo;
            std::vector<LateDeserializeInformation> outputLateDeserializeInfo;
            Connection* connection = Connection::deserialize(fileName, outputElement, newNode, inputLateDeserializeInfo, outputLateDeserializeInfo);
            if(connection == nullptr) {
                return nullptr;
            }
            lateDeserializeInputs[connection] = inputLateDeserializeInfo;
            lateDeserializeOutputs[connection] = outputLateDeserializeInfo;
            newNode->outputConnections.emplace_back(connection);
            outputElement = outputElement->NextSiblingElement("Connection");
        }
    }

    tinyxml2::XMLElement* extensionElement =  nodeElement->FirstChildElement("NodeExtension");
    if (extensionElement == nullptr) {
        if(newNode->getExtension() != nullptr) {
            std::cerr << "ERROR loading XML " << fileName << ": Node " << newNode->getName() << " has no extension info, using defaults." << std::endl;
        }//no need to log if no extension required, and none found
    } else {
        if(newNode->nodeExtension == nullptr) {
            std::cerr << "ERROR loading XML " << fileName << ": Node " << newNode->getName() << " has no extension, but one was serialized." << std::endl;
        } else {
            newNode->nodeExtension->deserialize(newNode->displayName, extensionElement);
        }
    }

    return newNode;
}

void Node::lateDeserialize(const std::unordered_map<Connection*, std::vector<LateDeserializeInformation>>& lateDeserializeInputs,
                           const std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeOutputs,
                           std::vector<Node *> allNodes) {
    for (auto input = inputConnections.begin(); input != inputConnections.end(); ++input) {
        if(lateDeserializeInputs.find((*input)) != lateDeserializeInputs.end()) {
            (*input)->lateSerialize(lateDeserializeInputs.at((*input)), allNodes);
        }
    }

    for (auto output = outputConnections.begin(); output != outputConnections.end(); ++output) {
        if(lateDeserializeOutputs.find((*output)) != lateDeserializeOutputs.end()) {
            (*output)->lateSerialize(lateDeserializeOutputs.at((*output)), allNodes);
        }
    }

    this->calculateAndSetDrawInformation();
}
