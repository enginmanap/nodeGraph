//
// Created by engin on 24.03.2019.
//

#include "Node.h"

std::vector<Node*> s_nodes;
uint32_t s_id = 0;

Node* createNodeFromType(ImVec2 pos, NodeType* nodeType)
{
    Node* node = new Node;
    node->id = s_id++;
    node->name = nodeType->name;

    ImVec2 titleSize = ImGui::CalcTextSize(node->name);

    titleSize.y *= 3;

    setupConnections(node->inputConnections, nodeType->inputConnections);
    setupConnections(node->outputConnections, nodeType->outputConnections);

    // Calculate the size needed for the whole box

    ImVec2 inputTextSize(0.0f, 0.0f);
    ImVec2 outputText(0.0f, 0.0f);

    for (Connection* c : node->inputConnections)
    {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);
        inputTextSize.x = std::max<float>(textSize.x, inputTextSize.x);

        c->pos = ImVec2(0.0f, titleSize.y + inputTextSize.y + textSize.y / 2.0f);

        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;		// size between text entries
    }

    inputTextSize.x += 40.0f;

    // max text size + 40 pixels in between

    float xStart = inputTextSize.x;

    // Calculate for the outputs

    for (Connection* c : node->outputConnections)
    {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);
        inputTextSize.x = std::max<float>(xStart + textSize.x, inputTextSize.x);
    }

    node->pos = pos;
    node->size.x = inputTextSize.x;
    node->size.y = inputTextSize.y + titleSize.y;

    inputTextSize.y = 0.0f;

    // set the positions for the output nodes when we know where the place them

    for (Connection* c : node->outputConnections)
    {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);

        c->pos = ImVec2(node->size.x, titleSize.y + inputTextSize.y + textSize.y / 2.0f);

        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;		// size between text entries
    }

    // calculate the size of the node depending on nuber of connections

    return node;
}

Node* createNodeFromName(ImVec2 pos, const char* name)
{
    for (int i = 0; i < (int)sizeof_array(s_nodeTypes); ++i)
    {
        if (!strcmp(s_nodeTypes[i].name, name))
            return createNodeFromType(pos, &s_nodeTypes[i]);
    }

    return 0;
}

Node* findNodeByCon(Connection* findCon)
{
    for (Node* node : s_nodes)
    {
        for (Connection* con : node->inputConnections)
        {
            if (con == findCon)
                return node;
        }

        for (Connection* con : node->outputConnections)
        {
            if (con == findCon)
                return node;
        }
    }

    return 0;
}