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


struct NodeType{
    const char* name;
    ConnectionDesc inputConnections[MAX_CONNECTION_COUNT];
    ConnectionDesc outputConnections[MAX_CONNECTION_COUNT];
};

static struct NodeType s_nodeTypes[] =
        {
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Math

                {
                        "Multiply",
                        // Input connections
                        {
                                { "Input1", ConnectionType_Float },
                                { "Input2", ConnectionType_Float },
                        },
                        // Output
                        {
                                { "Out", ConnectionType_Float },
                        },
                },

                {
                        "Add",
                        // Input connections
                        {
                                { "Input1", ConnectionType_Float },
                                { "Input2", ConnectionType_Float },
                        },
                        // Output
                        {
                                { "Out", ConnectionType_Float },
                        },
                },
        };

struct Node {
        ImVec2 pos;
        ImVec2 size;
        int id;
        const char* name;
        std::vector<Connection*> inputConnections;
        std::vector<Connection*> outputConnections;
};

extern std::vector<Node*> s_nodes;
extern uint32_t s_id;


Node* createNodeFromType(ImVec2 pos, NodeType* nodeType);

Node* createNodeFromName(ImVec2 pos, const char* name);

// TODO: Ugly fix: me
Node* findNodeByCon(Connection* findCon);


#endif //SIL_NODETEST_NODE_H
