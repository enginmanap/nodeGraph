//
// Created by engin on 24.03.2019.
//

#include "Node.h"

std::vector<Node *> s_nodes;
uint32_t s_id = 0;

Node::Node(ImVec2 pos, NodeType *nodeType) {
    initialize(pos, nodeType);

}

void Node::initialize(const ImVec2 &pos, const NodeType *nodeType) {
    id = s_id++;
    name = nodeType->name;

    ImVec2 titleSize = ImGui::CalcTextSize(name);

    titleSize.y *= 3;

    setupConnections(inputConnections, nodeType->inputConnections);
    setupConnections(outputConnections, nodeType->outputConnections);

    // Calculate the size needed for the whole box

    ImVec2 inputTextSize(0.0f, 0.0f);
    ImVec2 outputText(0.0f, 0.0f);

    for (Connection *c : inputConnections) {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);
        inputTextSize.x = std::max<float>(textSize.x, inputTextSize.x);

        c->pos = ImVec2(0.0f, titleSize.y + inputTextSize.y + textSize.y / 2.0f);

        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;        // size between text entries
    }

    inputTextSize.x += 40.0f;

    // max text size + 40 pixels in between

    float xStart = inputTextSize.x;

    // Calculate for the outputs

    for (Connection *c : outputConnections) {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);
        inputTextSize.x = std::max<float>(xStart + textSize.x, inputTextSize.x);
    }

    Node::pos = pos;
    size.x = inputTextSize.x;
    size.y = inputTextSize.y + titleSize.y;

    inputTextSize.y = 0.0f;

    // set the positions for the output nodes when we know where the place them

    for (Connection *c : outputConnections) {
        ImVec2 textSize = ImGui::CalcTextSize(c->desc.name);

        c->pos = ImVec2(size.x, titleSize.y + inputTextSize.y + textSize.y / 2.0f);

        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;        // size between text entries
    }

    // calculate the size of the node depending on nuber of connections
}

Node::Node(ImVec2 pos, const char *name, uint32_t &error) {
    for (int i = 0; i < (int) sizeof_array(s_nodeTypes); ++i) {
        if (!strcmp(s_nodeTypes[i].name, name)) {
            initialize(pos, &s_nodeTypes[i]);
            error = 0;
        }
    }
    error = 1;
}

Node *findNodeByCon(Connection *findCon) {
    for (Node *node : s_nodes) {
        for (Connection *con : node->inputConnections) {
            if (con == findCon)
                return node;
        }

        for (Connection *con : node->outputConnections) {
            if (con == findCon)
                return node;
        }
    }

    return 0;
}


void Node::display(ImDrawList *drawList, ImVec2 offset, int &node_selected) {
    int node_hovered_in_scene = -1;
    bool open_context_menu = false;

    ImGui::PushID(id);
    ImVec2 node_rect_min = offset + pos;

    // Display node contents first
    drawList->ChannelsSetCurrent(1); // Foreground
    bool old_any_active = ImGui::IsAnyItemActive();

    // Draw title in center

    ImVec2 textSize = ImGui::CalcTextSize(name);

    ImVec2 pos = node_rect_min + NODE_WINDOW_PADDING;
    pos.x = node_rect_min.x + (size.x / 2) - textSize.x / 2;

    ImGui::SetCursorScreenPos(pos);
    //ImGui::BeginGroup(); // Lock horizontal position
    ImGui::Text("%s", name);
    //ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
    //float dummy_color[3] = { node->Pos.x / ImGui::GetWindowWidth(), node->Pos.y / ImGui::GetWindowHeight(), fmodf((float)node->ID * 0.5f, 1.0f) };
    //ImGui::ColorEdit3("##color", &dummy_color[0]);

    // Save the size of what we have emitted and weither any of the widgets are being used
    bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
    //node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
    ImVec2 node_rect_max = node_rect_min + size;

    // Display node box
    drawList->ChannelsSetCurrent(0); // Background

    ImGui::SetCursorScreenPos(node_rect_min);
    ImGui::InvisibleButton("node", size);

    if (ImGui::IsItemHovered()) {
        node_hovered_in_scene = id;
        open_context_menu |= ImGui::IsMouseClicked(1);
    }

    bool node_moving_active = false;

    if (ImGui::IsItemActive() && !s_dragNode.con)
        node_moving_active = true;

    ImU32 node_bg_color = node_hovered_in_scene == id ? ImColor(75, 75, 75) : ImColor(60, 60, 60);
    drawList->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);

    ImVec2 titleArea = node_rect_max;
    titleArea.y = node_rect_min.y + 30.0f;

    // Draw text bg area
    drawList->AddRectFilled(node_rect_min + ImVec2(1, 1), titleArea, ImColor(100, 0, 0), 4.0f);
    drawList->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);

    ImVec2 off;

    offset.y += 40.0f;

    offset = offset + node_rect_min;

    off.x = node_rect_min.x;
    off.y = node_rect_min.y;

    for (Connection *con : inputConnections) {
        ImGui::SetCursorScreenPos(offset + ImVec2(10.0f, 0));
        ImGui::Text("%s", con->desc.name);

        ImColor conColor = ImColor(150, 150, 150);

        if (con->isHovered(node_rect_min))
            conColor = ImColor(200, 200, 200);

        drawList->AddCircleFilled(node_rect_min + con->pos, NODE_SLOT_RADIUS, conColor);

        offset.y += textSize.y + 2.0f;
    }

    offset = node_rect_min;
    offset.y += 40.0f;

    for (Connection *con : outputConnections) {
        textSize = ImGui::CalcTextSize(con->desc.name);

        ImGui::SetCursorScreenPos(offset + ImVec2(con->pos.x - (textSize.x + 10.0f), 0));
        ImGui::Text("%s", con->desc.name);

        ImColor conColor = ImColor(150, 150, 150);

        if (con->isHovered(node_rect_min))
            conColor = ImColor(200, 200, 200);

        drawList->AddCircleFilled(node_rect_min + con->pos, NODE_SLOT_RADIUS, conColor);

        offset.y += textSize.y + 2.0f;
    }


    //for (int i = 0; i < node->outputConnections.size(); ++i)
    //	drawList->AddCircleFilled(offset + node->outputSlotPos(i), NODE_SLOT_RADIUS, ImColor(150,150,150,150));

    if (node_widgets_active || node_moving_active)
        node_selected = id;

    if (node_moving_active && ImGui::IsMouseDragging(0))
        this->pos = this->pos + ImGui::GetIO().MouseDelta;

    //ImGui::EndGroup();

    ImGui::PopID();
}


void renderLines(ImDrawList *drawList, ImVec2 offset) {
    for (Node *node : s_nodes) {
        for (Connection *con : node->inputConnections) {
            if (!con->input)
                continue;

            Node *targetNode = findNodeByCon(con->input);

            if (!targetNode)
                continue;

            drawHermite(drawList,
                        offset + targetNode->pos + con->input->pos,
                        offset + node->pos + con->pos,
                        12);
        }
    }
}
