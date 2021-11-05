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

    /**
     * This is basically an equals operator, except it allows different NodeExtensions.
     * @param other     NodeType to compare to
     * @return          True if it is the same type, false if it is not
     */
    bool isSameNodeType(const NodeType& other) const {
        bool basicCheck = name == other.name &&
               editable == other.editable &&
               extensionName == other.extensionName &&
               combineInputs == other.combineInputs &&
               extraVariables == other.extraVariables;
        if(!basicCheck) {
            return false;
        }
        //now check the connections
        for (ConnectionDesc connectionDesc: inputConnections) {
            bool matchFound = false;
            for (ConnectionDesc otherConnectionDesc: other.inputConnections) {
                if(connectionDesc.name == otherConnectionDesc.name &&
                    connectionDesc.type == otherConnectionDesc.type
                ) {
                    matchFound = true;
                    break;
                }
            }
            if(!matchFound) {
                return false;
            }
        }
        for (ConnectionDesc connectionDesc: outputConnections) {
            bool matchFound = false;
            for (ConnectionDesc otherConnectionDesc: other.outputConnections) {
                if(connectionDesc.name == otherConnectionDesc.name &&
                   connectionDesc.type == otherConnectionDesc.type
                        ) {
                    matchFound = true;
                    break;
                }
            }
            if(!matchFound) {
                return false;
            }
        }
        return true;
    }
};


#endif //NODEEDITORSAMPLE_NODETYPE_H
