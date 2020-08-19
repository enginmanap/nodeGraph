//
// Created by engin on 24.03.2019.
//

#ifndef NODEGRAPH_NODE_H
#define NODEGRAPH_NODE_H

#include <vector>
#include <set>
#include <cstdint>
#include <tinyxml2.h>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "Connection.h"
#include "Common.h"
#include "NodeExtension.h"

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

struct NodeType {
    std::string name;
    bool editable;
    NodeExtension* nodeExtension = nullptr;
    std::vector<ConnectionDesc> inputConnections;
    std::vector<ConnectionDesc> outputConnections;
    bool combineInputs;

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
        tinyxml2::XMLElement * nodeTypeElement = document.NewElement("NodeType");
        parentElement->InsertEndChild(nodeTypeElement);

        tinyxml2::XMLElement * nameElement = document.NewElement("Name");
        nameElement->SetText(name.c_str());
        nodeTypeElement->InsertEndChild(nameElement);

        tinyxml2::XMLElement* combineInputsElement = document.NewElement("CombineInputs");
        combineInputsElement->SetText(combineInputs ? "True" : "False");
        nodeTypeElement->InsertEndChild(combineInputsElement);

        tinyxml2::XMLElement* editableElement = document.NewElement("Editable");
        editableElement->SetText(editable ? "True" : "False");
        nodeTypeElement->InsertEndChild(editableElement);

        tinyxml2::XMLElement * inputConnectionsElement = document.NewElement("Inputs");
        for (size_t i = 0; i < inputConnections.size(); ++i) {
            inputConnections[i].serialize(document, inputConnectionsElement);
        }
        nodeTypeElement->InsertEndChild(inputConnectionsElement);

        tinyxml2::XMLElement * outputConnectionsElement = document.NewElement("Outputs");
        for (size_t i = 0; i < inputConnections.size(); ++i) {
            inputConnections[i].serialize(document, outputConnectionsElement);
        }
        nodeTypeElement->InsertEndChild(outputConnectionsElement);

        if(nodeExtension != nullptr) {
            nodeExtension->serialize(document, nodeTypeElement);
        } else {
            tinyxml2::XMLElement * nodeExtensionElement = document.NewElement("NodeExtension");
            nodeExtensionElement->SetText("PlaceHolderExtension");
            nodeTypeElement->InsertEndChild(nodeExtensionElement);
        }
    }

    static NodeType *deserialize(const std::string &fileName,
            tinyxml2::XMLElement *nodeTypeElement,
            std::unordered_map<std::string, NodeExtension *> possibleNodeExtension) {
        NodeType* newNodeType = new NodeType();

        tinyxml2::XMLElement* nameElement =  nodeTypeElement->FirstChildElement("Name");
        if (nameElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Name of NodeType is not found!" << std::endl;
            exit(-1);
        }

        if(nameElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Name of NodeType has no text!" << std::endl;
            exit(-1);
        }
        newNodeType->name = nameElement->GetText();


        tinyxml2::XMLElement* extensionElement =  nodeTypeElement->FirstChildElement("NodeExtension");
        if (extensionElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Extension of NodeType is not found!" << std::endl;
        } else {
            if(extensionElement->GetText() != nullptr && std::string(extensionElement->GetText()) == "PlaceHolderExtension") {
                //no need to try to load the extension
            } else {
                tinyxml2::XMLElement *extensionNameElement = extensionElement->FirstChildElement("Name");
                if (extensionNameElement == nullptr) {
                    std::cerr << "Error loading XML " << fileName << ": Extension name of NodeType is not found!" << std::endl;
                } else {
                    if (extensionNameElement->GetText() == nullptr) {
                        std::cerr << "Error loading XML " << fileName << ": Extension name of NodeType has no text!" << std::endl;
                    } else {

                        if (possibleNodeExtension.find(extensionNameElement->GetText()) != possibleNodeExtension.end()) {
                            newNodeType->nodeExtension = possibleNodeExtension[extensionNameElement->GetText()];
                        } else {
                            std::cerr << "Error loading XML " << fileName << ": Extension of NodeType(" << extensionNameElement->GetText() << ") is not found, setting Null!"
                                      << std::endl;
                        }
                    }
                }
            }
        }
        tinyxml2::XMLElement* editableElement =  nodeTypeElement->FirstChildElement("Editable");
        if (editableElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Editable of NodeType is not found!" << std::endl;
            exit(-1);
        }
        if(editableElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Editable of NodeType has no text! assuming false" << std::endl;
            newNodeType->editable = false;
        } else {
            if(std::string(editableElement->GetText()) == "True") {
                newNodeType->editable = true;
            } else if(std::string(editableElement->GetText()) == "False") {
                newNodeType->editable = false;
            } else {
                std::cerr << "Error loading XML "<< fileName << ": Editable of NodeType has unknown("<<editableElement->GetText()<<") text! assuming false" << std::endl;
                newNodeType->editable = false;
            }
        }

        tinyxml2::XMLElement* combineInputsElement =  nodeTypeElement->FirstChildElement("CombineInputs");
        if (combineInputsElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Combine inputs of NodeType is not found!" << std::endl;
            exit(-1);
        }
        if(combineInputsElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Combine inputs of NodeType has no text! assuming false" << std::endl;
            newNodeType->combineInputs = false;
        } else {
            if(std::string(combineInputsElement->GetText()) == "True") {
                newNodeType->combineInputs = true;
            } else if(std::string(combineInputsElement->GetText()) == "False") {
                newNodeType->combineInputs = false;
            } else {
                std::cerr << "Error loading XML " << fileName << ": Combine inputs of NodeType has unknown(" << combineInputsElement->GetText() << ") text! assuming false" << std::endl;
                newNodeType->combineInputs = false;
            }
        }

        tinyxml2::XMLElement* inputsElement =  nodeTypeElement->FirstChildElement("Inputs");
        if (inputsElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Inputs of NodeType is not found!" << std::endl;
        }
        tinyxml2::XMLElement* inputElement =  inputsElement->FirstChildElement("ConnectionDesc");
        while(inputElement != nullptr) {
            ConnectionDesc inputDesc;
            ConnectionDesc::deserialize(fileName, inputElement, inputDesc);
            newNodeType->inputConnections.emplace_back(inputDesc);
            inputElement = inputElement->NextSiblingElement("ConnectionDesc");
        }

        tinyxml2::XMLElement* outputsElement =  nodeTypeElement->FirstChildElement("Outputs");
        if (outputsElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Outputs of NodeType is not found!" << std::endl;
        }
        tinyxml2::XMLElement* outputElement =  outputsElement->FirstChildElement("ConnectionDesc");
        while(outputElement != nullptr) {
            ConnectionDesc outputDesc;
            ConnectionDesc::deserialize(fileName, outputElement, outputDesc);
            newNodeType->outputConnections.emplace_back(outputDesc);
            outputElement = outputElement->NextSiblingElement("ConnectionDesc");
        }
        return newNodeType;
    }

};

class Node {
    ImVec2 pos;
    ImVec2 size;
    uint32_t id;
    uint32_t nextConnectionId;
    std::string name;
    std::vector<Connection *> inputConnections;
    std::vector<Connection *> outputConnections;
    bool editable;
    bool combineInputs;

    NodeExtension* nodeExtension = nullptr;

    void setupConnections(std::vector<Connection *> &connections, const std::vector<ConnectionDesc> &connectionDescs, Connection::Directions connectionType);

    void initialize(uint32_t id, const ImVec2 &pos, const NodeType *nodeType);

    /**
     * This method calculates node size and connection positions,
     * Node name or connection changes should trigger this method
     */
    void calculateAndSetDrawInformation();
public:
    Node(uint32_t id, ImVec2 pos, const NodeType *nodeType);

    Node(uint32_t id, ImVec2 pos, const std::vector<NodeType*> &allNodeTypes, const char *name, uint32_t &error);

    ~Node();

    bool isHovered(ImVec2 offset);//offset is scrool information

    void display(ImDrawList *drawList, ImVec2 offset, bool dragNodeConnected, bool isThisNodeSelected = false);

    bool hasConnection(Connection *connection);

    Connection* getHoverConnection(ImVec2 offset, ImVec2 *pos);

    std::vector<std::pair<ImVec2, ImVec2>> getLinesToRender();

    const std::string& getName() const {
        return name;
    }

    uint32_t getId() const {
        return id;
    }

    void setName(const std::string& name) {
        this->name = name;
        calculateAndSetDrawInformation();
    }

    void addInput(const ConnectionDesc &description);
    void removeInput(Connection *connection);

    void addOutput(const ConnectionDesc &description);
    void removeOutput(Connection *connection);
    void removeAllOutputs();
    bool getEditable() {
        return this->editable;
    }

    std::set<Node*> getOutputConnectedNodes();

    std::vector<const Connection *> getOutputConnections() const {
        std::vector<const Connection *> result;
        for (size_t i = 0; i < outputConnections.size(); ++i) {
            result.push_back(outputConnections.at(i));
        }
        return result;
    }

    std::vector<const Connection *> getInputConnections() const {
        std::vector<const Connection *> result;
        for (size_t i = 0; i < inputConnections.size(); ++i) {
            result.push_back(inputConnections.at(i));
        }
        return result;
    }

    NodeExtension* getExtension() const {
        return nodeExtension;
    }

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement);
    static Node *deserialize(const std::string &fileName,
                             tinyxml2::XMLElement *nodeElement,
                             std::vector<NodeType*> possibleNodeTypes,
                             std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeInputs,
                             std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeOutputs);

    void lateDeserialize(const std::unordered_map<Connection*, std::vector<LateDeserializeInformation>>& lateDeserializeInputs,
                         const std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> &lateDeserializeOutputs,
                         std::vector<Node *> allNodes);
};


#endif //NODEGRAPH_NODE_H
