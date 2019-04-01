//
// Created by engin on 24.03.2019.
//

#ifndef SIL_NODETEST_NODE_H
#define SIL_NODETEST_NODE_H

#include <vector>
#include <cstdint>

#include "../imgui/imgui.h"
#include "Connection.h"
#include "Common.h"

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

struct NodeType {
    const char *name;
    ConnectionDesc inputConnections[MAX_CONNECTION_COUNT];
    ConnectionDesc outputConnections[MAX_CONNECTION_COUNT];
};

static struct NodeType s_nodeTypes[] =
        {
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Math
                {
                        "Texture",
                        // Input connections
                        {
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Vec3},
                        },
                },
                {
                        "Multiply",
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Float},
                        },
                },

                {
                        "Add",
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Float},
                        },
                },

                {
                        "Divide",
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Output1", ConnectionType_Float},
                                {"Output2", ConnectionType_Float},
                                {"Output3", ConnectionType_Float},
                        },
                },

        };

class Node {
    ImVec2 pos;
    ImVec2 size;
    int id;
    std::string name;
    std::vector<Connection *> inputConnections;
    std::vector<Connection *> outputConnections;

    void setupConnections(std::vector<Connection *> &connections, const ConnectionDesc *connectionDescs, Connection::Types connectionType);
public:
    Node(uint32_t id, ImVec2 pos, NodeType *nodeType);

    Node(uint32_t id, ImVec2 pos, const char *name, uint32_t &error);

    bool isHovered(ImVec2 offset);//offset is scrool information

    void initialize(uint32_t id, const ImVec2 &pos, const NodeType *nodeType);

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

    void addOutput(const ConnectionDesc &description);


    /**
     * This method calculates node size and connection positions,
     * Node name or connection changes should trigger this method
     */
    void calculateAndSetDrawInformation();
};


#endif //SIL_NODETEST_NODE_H
