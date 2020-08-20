//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_SAMPLEEXTENSION_H
#define NODEGRAPH_SAMPLEEXTENSION_H


#include <vector>
#include <iostream>
#include "../src/NodeExtension.h"

class SampleExtension : public NodeExtension {
    enum class PossibleTypes { NONE, INT, FLOAT, LONG, STRING };
    PossibleTypes type;

    std::string getNameOfType(PossibleTypes processingType) {
        switch (processingType) {
            case PossibleTypes::NONE: return "None";
            case PossibleTypes::INT: return "Integer";
            case PossibleTypes::FLOAT: return "Float";
            case PossibleTypes::LONG: return "Long";
            case PossibleTypes::STRING: return "String";
        }
    }
    std::vector<PossibleTypes> getAllValidTypes() {
        return std::vector<PossibleTypes>({PossibleTypes::INT, PossibleTypes::FLOAT, PossibleTypes::LONG, PossibleTypes::STRING});
    }

public:

    SampleExtension() {
        std::cout << "Construction of Sample Extension" << std::endl;
    }

    void drawDetailPane(Node *node) override;

    bool isConnectionActive(Connection* connection) override;

    std::string getName() override {
        return "SampleExtension";
    }

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) override {
        tinyxml2::XMLElement *nodeTypeElement = document.NewElement("NodeExtension");
        parentElement->InsertEndChild(nodeTypeElement);

        tinyxml2::XMLElement *nameElement = document.NewElement("Name");
        nameElement->SetText(getName().c_str());
        nodeTypeElement->InsertEndChild(nameElement);

        tinyxml2::XMLElement *typeElement = document.NewElement("Type");
        typeElement->SetText(getNameOfType(type).c_str());
        nodeTypeElement->InsertEndChild(typeElement);
    }

    void deserialize(const std::string &fileName, tinyxml2::XMLElement *nodeExtensionElement) override {
        tinyxml2::XMLElement* typeElement =  nodeExtensionElement->FirstChildElement("Type");
        if (typeElement == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Type of Node Extension is not found!" << std::endl;
            exit(-1);
        }

        if(typeElement->GetText() == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Type of Node Extension has no text!" << std::endl;
            exit(-1);
        }
        std::string typeString = typeElement->GetText();
        std::vector<PossibleTypes> allValidTypes = getAllValidTypes();
        for(auto iterator = allValidTypes.begin(); iterator != allValidTypes.end(); ++iterator) {
            if(getNameOfType(*iterator) == typeString) {
                this->type = *iterator;
                return;
            }
        }
        std::cerr << "Error loading XML "<< fileName << ": Type of Node Extension is unknown!" << std::endl;
        exit(-1);
    }

};


#endif //NODEGRAPH_SAMPLEEXTENSION_H
