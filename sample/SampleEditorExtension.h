//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_SAMPLEEDITOREXTENSION_H
#define NODEGRAPH_SAMPLEEDITOREXTENSION_H


#include <string>
#include <iostream>
#include "../src/EditorExtension.h"

class SampleEditorExtension : public EditorExtension {
    char inputBuffer[256];
public:
    void drawDetailPane(NodeGraph* nodeGraph, const std::vector<const Node *>& nodes, const Node* selectedNode = nullptr) override;

    std::string getName() {
        return "SampleEditorExtension";
    }

    virtual void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
        tinyxml2::XMLElement *nodeTypeElement = document.NewElement("EditorExtension");
        parentElement->InsertEndChild(nodeTypeElement);

        tinyxml2::XMLElement *nameElement = document.NewElement("Name");
        nameElement->SetText(getName().c_str());
        nodeTypeElement->InsertEndChild(nameElement);


        tinyxml2::XMLElement *customInputElement = document.NewElement("CustomInput");
        customInputElement->SetText(inputBuffer);
        nodeTypeElement->InsertEndChild(customInputElement);

    }

    void deserialize(const std::string &fileName, tinyxml2::XMLElement *editorExtensionElement) override {
        tinyxml2::XMLElement* customInput =  editorExtensionElement->FirstChildElement("CustomInput");
        if (customInput == nullptr) {
            std::cerr << "Error loading XML "<< fileName << ": Custom input of Editor Extension is not found!" << std::endl;
        } else {
            if (customInput->GetText() == nullptr) {
                std::cerr << "Error loading XML " << fileName << ": Custom input of Editor Extension has no text!" << std::endl;
            } else {
                strncpy(inputBuffer, customInput->GetText(), sizeof(inputBuffer));
            }
        }
    }
};


#endif //NODEGRAPH_SAMPLEEDITOREXTENSION_H
