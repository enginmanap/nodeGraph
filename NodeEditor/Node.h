//
// Created by engin on 24.03.2019.
//

#ifndef NODEGRAPH_NODE_H
#define NODEGRAPH_NODE_H

#include <vector>
#include <set>
#include <cstdint>

#include "../imgui/imgui.h"
#include "Connection.h"
#include "Common.h"

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

struct NodeType {
    const char *name;
    bool editable;
    ConnectionDesc inputConnections[MAX_CONNECTION_COUNT];
    ConnectionDesc outputConnections[MAX_CONNECTION_COUNT];
};

class Node {
    ImVec2 pos;
    ImVec2 size;
    int id;
    std::string name;
    std::vector<Connection *> inputConnections;
    std::vector<Connection *> outputConnections;
    bool editable;

    void setupConnections(std::vector<Connection *> &connections, const ConnectionDesc *connectionDescs, Connection::Directions connectionType);

    void initialize(uint32_t id, const ImVec2 &pos, const NodeType *nodeType);

    /**
     * This method calculates node size and connection positions,
     * Node name or connection changes should trigger this method
     */
    void calculateAndSetDrawInformation();
public:
    Node(uint32_t id, ImVec2 pos, const NodeType *nodeType);

    Node(uint32_t id, ImVec2 pos, const std::vector<NodeType> &allNodeTypes, const char *name, uint32_t &error);

    ~Node();

    bool isHovered(ImVec2 offset);//offset is scrool information

    void display(ImDrawList *drawList, ImVec2 offset, bool dragNodeConnected);

    bool hasConnection(Connection *connection);

    Connection* getHoverConnection(ImVec2 offset, ImVec2 *pos);

    std::vector<std::pair<ImVec2, ImVec2>> getLinesToRender();

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void addInput(const ConnectionDesc &description);
    void removeInput(Connection *connection);

    void addOutput(const ConnectionDesc &description);
    void removeOutput(Connection *connection);
    bool getEditable() {
        return this->editable;
    }

    std::set<Node*> getOutputConnectedNodes();
};


#endif //NODEGRAPH_NODE_H
