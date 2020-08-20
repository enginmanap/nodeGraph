//
// Created by engin on 20.08.2020.
//

#include "NodeType.h"


void NodeType::serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
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

    tinyxml2::XMLElement * nodeExtensionElement = document.NewElement("NodeExtension");
    if(extensionName.empty() ) {
        nodeExtensionElement->SetText("PlaceHolderExtension");
    } else {
        nodeExtensionElement->SetText(extensionName.c_str());
    }
    nodeTypeElement->InsertEndChild(nodeExtensionElement);
}

NodeType *NodeType::deserialize(const std::string &fileName,
                             tinyxml2::XMLElement *nodeTypeElement,
                             std::unordered_map<std::string, NodeExtension*(*)()> possibleNodeExtension) {
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
        if(extensionElement->GetText() == nullptr) {
            std::cerr << "NodeType extension information not found, this should not have happened." << std::endl;
        } else {
            std::string nodeExtensionName = std::string(extensionElement->GetText());
            if(nodeExtensionName  == "PlaceHolderExtension") {
                //no need to try to load;
            } else {
                if (possibleNodeExtension.find(nodeExtensionName) != possibleNodeExtension.end()) {
                    newNodeType->extensionName = nodeExtensionName;
                    newNodeType->nodeExtensionConstructor = possibleNodeExtension[nodeExtensionName];
                } else {
                    std::cerr << "Error loading XML " << fileName << ": Extension of NodeType(" << nodeExtensionName << ") is not found, setting Null!"
                              << std::endl;
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
    } else {
        tinyxml2::XMLElement *inputElement = inputsElement->FirstChildElement("ConnectionDesc");
        while (inputElement != nullptr) {
            ConnectionDesc inputDesc;
            ConnectionDesc::deserialize(fileName, inputElement, inputDesc);
            newNodeType->inputConnections.emplace_back(inputDesc);
            inputElement = inputElement->NextSiblingElement("ConnectionDesc");
        }
    }
    tinyxml2::XMLElement* outputsElement =  nodeTypeElement->FirstChildElement("Outputs");
    if (outputsElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Outputs of NodeType is not found!" << std::endl;
    } else {
        tinyxml2::XMLElement *outputElement = outputsElement->FirstChildElement("ConnectionDesc");
        while (outputElement != nullptr) {
            ConnectionDesc outputDesc;
            ConnectionDesc::deserialize(fileName, outputElement, outputDesc);
            newNodeType->outputConnections.emplace_back(outputDesc);
            outputElement = outputElement->NextSiblingElement("ConnectionDesc");
        }
    }
    return newNodeType;
}