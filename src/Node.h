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
#include "NodeType.h"

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

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

    bool isHovered(ImVec2 offset);//offset is scroll information

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
