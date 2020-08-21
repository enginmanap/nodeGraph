//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_EDITOREXTENSION_H
#define NODEGRAPH_EDITOREXTENSION_H

#include <vector>
#include <tinyxml2.h>

class Node;

class EditorExtension {
public:
    virtual ~EditorExtension() {}

    virtual void drawDetailPane(const std::vector<const Node *>& nodes, const Node* selectedNode = nullptr) = 0;

    virtual std::string getName() = 0;

    virtual void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
        tinyxml2::XMLElement *nodeTypeElement = document.NewElement("EditorExtension");
        parentElement->InsertEndChild(nodeTypeElement);

        tinyxml2::XMLElement *nameElement = document.NewElement("Name");
        nameElement->SetText(getName().c_str());
        nodeTypeElement->InsertEndChild(nameElement);
    }

    virtual void deserialize(const std::string &fileName [[gnu::unused]],
                             tinyxml2::XMLElement *editorExtensionElement [[gnu::unused]]) {
        //intentionally empty
    }

};


#endif //NODEGRAPH_EDITOREXTENSION_H
