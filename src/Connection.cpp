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

/**
 * Display should get information about the offset from Node,
 * and it should provide offset.
 *
 * This is because text of the connection is not known, so size needs calculation.
 */
void Connection::display(ImDrawList *drawList, const ImVec2 node_rect_min, ImVec2 &offset, ImVec2 &textSize) {
    switch (direction) {
        case Directions::INPUT: {
            ImGui::SetCursorScreenPos(offset + ImVec2(10.0f, 0));
            ImGui::Text("%s", this->desc.name.c_str());

            ImColor conColor = ImColor(150, 150, 150);

            if (this->isHovered(node_rect_min))
                conColor = ImColor(200, 200, 200);

            drawList->AddCircleFilled(node_rect_min + this->pos, NODE_SLOT_RADIUS, conColor);

            offset.y += textSize.y + 4.0f;
        }
        break;
        case Directions::OUTPUT: {
            textSize = ImGui::CalcTextSize(this->desc.name.c_str());

            ImGui::SetCursorScreenPos(offset + ImVec2(this->pos.x - (textSize.x + 10.0f), 0));
            ImGui::Text("%s", this->desc.name.c_str());

            ImColor conColor = ImColor(150, 150, 150);

            if (this->isHovered(node_rect_min))
                conColor = ImColor(200, 200, 200);

            drawList->AddCircleFilled(node_rect_min + this->pos, NODE_SLOT_RADIUS, conColor);

            offset.y += textSize.y + 4.0f;
        }
        break;
    }
}

void Connection::displayDataTooltip() const {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
    ImGui::Text(" %s ",this->desc.type.c_str());
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

void Connection::setPosition(float TitleSizeY, float& textSizeY, float xPosition) {
    ImVec2 textSize = this->getTextSize();

    this->pos = ImVec2(xPosition, TitleSizeY + textSizeY + textSize.y / 2.0f);

    textSizeY += textSize.y;
    textSizeY += 4.0f;        // size between text entries
}

Connection::~Connection() {
    clearConnections();
}

void Connection::clearConnections() {
    if(!this->inputList.empty()) {
        for(auto currentInput:this->inputList) {
            for (auto it = currentInput->outputList.begin(); it != currentInput->outputList.end(); it++) {
                if (*it == this) {
                    currentInput->outputList.erase(it);
                    break;
                }
            }
        }
    }
    this->inputList.clear();

    for(auto & it : this->outputList) {
        it->removeInputConnection(this);
    }
    this->outputList.clear();
}

std::vector<Node*> Connection::getConnectedNodes() const {
    std::vector<Node*> nodes;
    switch(this->direction) {
        case Directions::OUTPUT:  {
            for(Connection* con:this->outputList) {
                nodes.push_back(con->parent);
            }
        }
        break;
        case Directions::INPUT:  {
            for(auto connection:inputList) {
                nodes.push_back(connection->parent);
            }
        }
        break;
    }
    return nodes;
}

void Connection::serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
    tinyxml2::XMLElement *connectionElement = document.NewElement("Connection");
    parentElement->InsertEndChild(connectionElement);

    tinyxml2::XMLElement *idElement = document.NewElement("ID");
    idElement->SetText(std::to_string(id).c_str());
    connectionElement->InsertEndChild(idElement);

    tinyxml2::XMLElement *nameElement = document.NewElement("Name");
    nameElement->SetText(desc.name.c_str());
    connectionElement->InsertEndChild(nameElement);

    tinyxml2::XMLElement *typeElement = document.NewElement("Type");
    typeElement->SetText(desc.type.c_str());
    connectionElement->InsertEndChild(typeElement);

    tinyxml2::XMLElement *directionElement = document.NewElement("Direction");
    switch (direction) {
        case Directions::INPUT:
            directionElement->SetText("Input");
            break;
        case Directions::OUTPUT:
            directionElement->SetText("Output");
            break;
        default:
            std::cerr << "Unknown direction, assuming output!" << std::endl;
    }
    connectionElement->InsertEndChild(directionElement);

    tinyxml2::XMLElement *combinedInputsElement = document.NewElement("CombineInputs");
    combinedInputsElement->SetText(combinedInput ? "True" : "False");
    connectionElement->InsertEndChild(combinedInputsElement);


    tinyxml2::XMLElement *positionElement = document.NewElement("Position");
    connectionElement->InsertEndChild(positionElement);
    tinyxml2::XMLElement *posXElement = document.NewElement("X");
    posXElement->SetText(std::to_string(pos.x).c_str());
    positionElement->InsertEndChild(posXElement);
    tinyxml2::XMLElement *posYElement = document.NewElement("Y");
    posYElement->SetText(std::to_string(pos.y).c_str());
    positionElement->InsertEndChild(posYElement);

    tinyxml2::XMLElement *allInputsElement = document.NewElement("Inputs");
    for (auto & i : inputList) {
        tinyxml2::XMLElement *inputElement = document.NewElement("Input");
        inputElement->SetText(i->getName().c_str());
        inputElement->SetAttribute("NodeId", i->getParent()->getId());
        inputElement->SetAttribute("ConnectionID", i->getId());
        allInputsElement->InsertEndChild(inputElement);
    }
    connectionElement->InsertEndChild(allInputsElement);

    tinyxml2::XMLElement *allOutputsElement = document.NewElement("Outputs");
    for (auto & i : outputList) {
        tinyxml2::XMLElement *outputElement = document.NewElement("Output");
        outputElement->SetText(i->getName().c_str());
        outputElement->SetAttribute("NodeId", i->getParent()->getId());
        outputElement->SetAttribute("ConnectionID", i->getId());
        allOutputsElement->InsertEndChild(outputElement);
    }
    connectionElement->InsertEndChild(allOutputsElement);

}

Connection* Connection::deserialize(const std::string &fileName,
        tinyxml2::XMLElement *connectionElement, Node* parentNode,
        std::vector<LateDeserializeInformation>& inputs,
        std::vector<LateDeserializeInformation>& outputs) {
    tinyxml2::XMLElement *idElement = connectionElement->FirstChildElement("ID");
    if (idElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": ID of Connection is not found!" << std::endl;
        return nullptr;
    }

    if (idElement->GetText() == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": ID of Connection has no text!" << std::endl;
        return nullptr;
    }
    uint32_t connectionID = std::stoul(idElement->GetText());

    tinyxml2::XMLElement *nameElement = connectionElement->FirstChildElement("Name");
    if (nameElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Name of Connection is not found!" << std::endl;
        return nullptr;
    }

    if (nameElement->GetText() == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Name of Connection has no text!" << std::endl;
        return nullptr;
    }
    std::string connectionName = nameElement->GetText();

    tinyxml2::XMLElement *typeElement = connectionElement->FirstChildElement("Type");
    if (typeElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Type of Connection is not found!" << std::endl;
        return nullptr;
    }

    if (typeElement->GetText() == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Type of Connection has no text!" << std::endl;
        return nullptr;
    }
    std::string type = typeElement->GetText();

    tinyxml2::XMLElement *combineInputsElement = connectionElement->FirstChildElement("CombineInputs");
    if (combineInputsElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Type of Connection is not found!" << std::endl;
        return nullptr;
    }

    if (combineInputsElement->GetText() == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Type of Connection has no text!" << std::endl;
        return nullptr;
    }

    bool combineInputs;
    if ("True" == std::string(combineInputsElement->GetText())) {
        combineInputs = true;
    } else if ("False" == std::string(combineInputsElement->GetText())) {
        combineInputs = false;
    } else {
        std::cerr << "Error loading XML " << fileName << ": CombineInputs of Connection (" << combineInputsElement->GetText() << ") is unknown, failing!" << std::endl;
        return nullptr;
    }

    tinyxml2::XMLElement *directionElement = connectionElement->FirstChildElement("Direction");
    if (directionElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Direction of Connection is not found!" << std::endl;
        return nullptr;
    }

    if (directionElement->GetText() == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Direction of Connection has no text!" << std::endl;
        return nullptr;
    }
    std::string directionString = directionElement->GetText();

    Directions newDirection;
    if (directionString == "Input") {
        newDirection = Directions::INPUT;
    } else if (directionString == "Output") {
        newDirection = Directions::OUTPUT;
    } else {
        std::cerr << "Error loading XML " << fileName << ": Direction of Connection (" << directionString << ") is unknown, failing!" << std::endl;
        return nullptr;
    }

    ConnectionDesc tempDescription;

    Connection *newConnection = new Connection(parentNode, connectionID, tempDescription, newDirection, combineInputs);

    newConnection->desc.name = connectionName;
    newConnection->desc.type = type;

    ImVec2 position;
    tinyxml2::XMLElement *positionElement = connectionElement->FirstChildElement("Position");
    if (positionElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Position of Node is not found!" << std::endl;
        return nullptr;
    } else {

        tinyxml2::XMLElement *posXElement = positionElement->FirstChildElement("X");
        if (posXElement == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": X position of Node is not found!" << std::endl;
            return nullptr;
        }

        if (posXElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": X position of Node has no text!" << std::endl;
            return nullptr;
        }
        position.x = std::stof(posXElement->GetText());

        tinyxml2::XMLElement *posYElement = positionElement->FirstChildElement("Y");
        if (posYElement == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Y position of Node is not found!" << std::endl;
            return nullptr;
        }

        if (posYElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Y position of Node has no text!" << std::endl;
            return nullptr;
        }
        position.y = std::stof(posYElement->GetText());
    }

    tinyxml2::XMLElement *allInputsElement = connectionElement->FirstChildElement("Inputs");
    if (allInputsElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": Inputs of Connection is not found!" << std::endl;
        return nullptr;
    }

    tinyxml2::XMLElement *inputElement = allInputsElement->FirstChildElement("Input");
    while (inputElement != nullptr) {
        LateDeserializeInformation inputLateDeserializeInformation;
        if (inputElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Name of Connection input is not found!" << std::endl;
            return nullptr;
        } else {
            inputLateDeserializeInformation.connectionName = inputElement->GetText();
        }

        if (inputElement->Attribute("NodeId") == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": NodeId of Connection input is not found!" << std::endl;
            return nullptr;
        }
        inputLateDeserializeInformation.nodeID = std::stoul(inputElement->Attribute("NodeId"));

        if (inputElement->Attribute("ConnectionID") == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": ConnectionID of Connection input is not found!" << std::endl;
            return nullptr;
        }
        inputLateDeserializeInformation.connectionID = std::stoul(inputElement->Attribute("ConnectionID"));
        inputs.emplace_back(inputLateDeserializeInformation);

        inputElement = inputElement->NextSiblingElement("Input");
    }

    tinyxml2::XMLElement* AllOutputsElement =  connectionElement->FirstChildElement("Outputs");
    if (AllOutputsElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Outputs of Connection is not found!" << std::endl;
        return nullptr;
    }
    tinyxml2::XMLElement *outputElement = allInputsElement->FirstChildElement("Output");
    while (outputElement != nullptr) {
        LateDeserializeInformation outputLateDeserializeInformation;
        if (outputElement->GetText() == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": Name of Connection output is not found!" << std::endl;
            return nullptr;
        } else {
            outputLateDeserializeInformation.connectionName = outputElement->GetText();
        }

        if (outputElement->Attribute("NodeId") == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": NodeId of Connection output is not found!" << std::endl;
            return nullptr;
        }
        outputLateDeserializeInformation.nodeID = std::stoul(outputElement->Attribute("NodeId"));

        if (outputElement->Attribute("ConnectionID") == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": For connection (" << parentNode->getName() << "|" << newConnection->getName()
                      << ") ConnectionId of Connection output is not found!" << std::endl;
            return nullptr;
        }
        //outputLateDeserializeInformation.connectionName = std::stoul(outputsElement->Attribute("ConnectionID"));
        outputs.emplace_back(outputLateDeserializeInformation);

        outputElement = outputElement->NextSiblingElement("Output");
    }
    newConnection->pos = position;
    return newConnection;
}

void Connection::lateSerialize(const std::vector<LateDeserializeInformation> &lateDeserializeList, std::vector<Node *> allNodes) {
    for (auto lateDeserializeInfo = lateDeserializeList.begin(); lateDeserializeInfo != lateDeserializeList.end(); ++lateDeserializeInfo) {
        for (auto node = allNodes.begin(); node != allNodes.end(); ++node) {
            if((*node)->getId() == lateDeserializeInfo->nodeID) {
                if(this->direction == Directions::INPUT) { // only handling inputs, because they automatically handle outputs.
                    auto otherNodeOutputs = (*node)->getOutputConnections();
                    for (auto otherConnection = otherNodeOutputs.begin(); otherConnection != otherNodeOutputs.end(); ++otherConnection) {
                        if((*otherConnection)->getId() == lateDeserializeInfo->connectionID &&
                                (*otherConnection)->getName() == lateDeserializeInfo->connectionName) {
                            this->addInputConnection(const_cast<Connection*>(*otherConnection));
                        }
                    }
                }
            }
        }

    }
}

