//
// Created by engin on 20.08.2020.
//

#ifndef NODEEDITORSAMPLE_NODETYPE_H
#define NODEEDITORSAMPLE_NODETYPE_H

#include <vector>
#include <tinyxml2.h>
#include <unordered_map>
#include <map>

#include "Connection.h"
#include "NodeExtension.h"

struct NodeType {
    std::string name;
    bool editable;
    std::string extensionName;
    std::function<NodeExtension*(const NodeType*)> nodeExtensionConstructor = [](const NodeType* type[[gnu::unused]]= nullptr) -> NodeExtension* {return nullptr;};
    std::vector<ConnectionDesc> inputConnections;
    std::vector<ConnectionDesc> outputConnections;
    bool combineInputs;
    std::map<std::string, std::string> extraVariables;//used for keeping extra variables per type


    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement);

    static NodeType *deserialize(const std::string &fileName,
                                 tinyxml2::XMLElement *nodeTypeElement,
                                 std::unordered_map<std::string, std::function<NodeExtension*(const NodeType*)>> possibleNodeExtension);

};


#endif //NODEEDITORSAMPLE_NODETYPE_H
