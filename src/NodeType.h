//
// Created by engin on 20.08.2020.
//

#ifndef NODEEDITORSAMPLE_NODETYPE_H
#define NODEEDITORSAMPLE_NODETYPE_H

#include <vector>
#include <tinyxml2.h>
#include <unordered_map>

#include "Connection.h"
#include "NodeExtension.h"

struct NodeType {
    std::string name;
    bool editable;
    NodeExtension* nodeExtension = nullptr;
    std::vector<ConnectionDesc> inputConnections;
    std::vector<ConnectionDesc> outputConnections;
    bool combineInputs;

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement);

    static NodeType *deserialize(const std::string &fileName,
                                 tinyxml2::XMLElement *nodeTypeElement,
                                 std::unordered_map<std::string, NodeExtension*(*)()> possibleNodeExtension);

};


#endif //NODEEDITORSAMPLE_NODETYPE_H
