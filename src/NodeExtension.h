//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_NODEEXTENSION_H
#define NODEGRAPH_NODEEXTENSION_H

#include <string>
#include <tinyxml2.h>
#include <functional>

class Node;
class NodeType;
class Connection;

class NodeExtension {
public:
    virtual void drawDetailPane(Node* node) = 0;

    NodeExtension(const NodeType* type[[gnu::unused]]) {}//we expect extensions to use NodeType. If sure user can pass null
    virtual ~NodeExtension() {}

    virtual bool isConnectionActive(Connection* connection [[gnu::unused]]) { return true;}

    virtual std::string getName() = 0;

    virtual void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) {
        tinyxml2::XMLElement *nodeTypeElement = document.NewElement("NodeExtension");
        parentElement->InsertEndChild(nodeTypeElement);

        tinyxml2::XMLElement *nameElement = document.NewElement("Name");
        nameElement->SetText(getName().c_str());
        nodeTypeElement->InsertEndChild(nameElement);
    }

    virtual void deserialize(const std::string &nodeName [[gnu::unused]],
                             tinyxml2::XMLElement *nodeExtensionElement [[gnu::unused]]) {
        //intentionally empty
    }
};


#endif //NODEGRAPH_NODEEXTENSION_H
