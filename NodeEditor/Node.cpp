//
// Created by engin on 24.03.2019.
//

#include "Node.h"

Node::Node(uint32_t id, ImVec2 pos, NodeType *nodeType) {
    initialize(id, pos, nodeType);
}

Node::Node(uint32_t id, ImVec2 pos, const char *name, uint32_t &error) {
    for (int i = 0; i < (int) sizeof_array(s_nodeTypes); ++i) {
        if (!strcmp(s_nodeTypes[i].name, name)) {
            initialize(id, pos, &s_nodeTypes[i]);
            error = 0;
        }
    }
    error = 1;
}

void Node::initialize(uint32_t id, const ImVec2 &pos, const NodeType *nodeType) {
    this->id = id;
    this->pos = pos;
    this->name = nodeType->name;
    this->editable = nodeType->editable;

    setupConnections(inputConnections, nodeType->inputConnections, Connection::Directions::INPUT);
    setupConnections(outputConnections, nodeType->outputConnections, Connection::Directions::OUTPUT);
    calculateAndSetDrawInformation();

}

void Node::calculateAndSetDrawInformation() {
    // Calculate the size needed for the whole box

    ImVec2 titleSize = ImGui::CalcTextSize(name.c_str());

    titleSize.y *= 3;
    titleSize.x += 5;

    ImVec2 inputTextSize(0.0f, 0.0f);
    ImVec2 outputTextSize(0.0f, 0.0f);

    for (Connection *c : inputConnections) {
        ImVec2 textSize = c->getTextSize();
        inputTextSize.x = std::max<float>(textSize.x, inputTextSize.x);
        inputTextSize.y += textSize.y;
        inputTextSize.y += 4.0f;        // size between text entries
    }
    inputTextSize.x += 40.0f;

    // max text size + 40 pixels in between
    float xStart = inputTextSize.x;

    // Calculate for the outputs
    for (Connection *c : outputConnections) {
        ImVec2 textSize = c->getTextSize();
        inputTextSize.x = std::max<float>(xStart + textSize.x, inputTextSize.x);
        outputTextSize.y += textSize.y;
        outputTextSize.y += 4.0f;        // size between text entries
    }


    size.x = std::max(titleSize.x, inputTextSize.x);
    size.y = std::max(inputTextSize.y, outputTextSize.y) + titleSize.y;

    inputTextSize.y = 0.0f;

    for (Connection *c : inputConnections) {
        c->setPosition(titleSize.y, inputTextSize.y, 0);
    }

    // set the positions for the output nodes when we know where the place them
    outputTextSize.y = 0.0f;

    for (Connection *c : outputConnections) {
        c->setPosition(titleSize.y, outputTextSize.y, size.x);
    }

    // calculate the size of the node depending on nuber of connections
}

void Node::setupConnections(std::vector<Connection *> &connections, const ConnectionDesc *connectionDescs, Connection::Directions connectionType) {
    for (int i = 0; i < MAX_CONNECTION_COUNT; ++i) {
        const ConnectionDesc &desc = connectionDescs[i];

        if (desc.name.empty())
            break;

        Connection *con = new Connection(this, desc, connectionType);

        connections.push_back(con);
    }
}

void Node::display(ImDrawList *drawList, ImVec2 offset, bool dragNodeConnected) {
    int node_hovered_in_scene = -1;
    bool open_context_menu = false;

    ImGui::PushID(id);
    ImVec2 node_rect_min = offset + pos;

    // Display node contents first
    drawList->ChannelsSetCurrent(1); // Foreground
    bool old_any_active = ImGui::IsAnyItemActive();

    // Draw title in center

    ImVec2 textSize = ImGui::CalcTextSize(name.c_str());

    ImVec2 pos = node_rect_min + NODE_WINDOW_PADDING;
    pos.x = node_rect_min.x + (size.x / 2) - textSize.x / 2;

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

    if (ImGui::IsItemActive() && !dragNodeConnected)
        node_moving_active = true;

    ImU32 node_bg_color = node_hovered_in_scene == id ? ImColor(75, 75, 75) : ImColor(60, 60, 60);
    drawList->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);

    ImVec2 titleArea = node_rect_max;
    titleArea.y = node_rect_min.y + 30.0f;

    // Draw text bg area
    drawList->AddRectFilled(node_rect_min + ImVec2(1, 1), titleArea, ImColor(100, 0, 0), 4.0f);
    drawList->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 4.0f);


    offset = node_rect_min;
    offset.y += 40.0f;

    ImGui::SetCursorScreenPos(pos);
    //ImGui::BeginGroup(); // Lock horizontal position
    ImGui::Text("%s", name.c_str());

    for (Connection *con : inputConnections) {
        con->display(drawList, node_rect_min, offset, textSize);
    }

    offset = node_rect_min;
    offset.y += 40.0f;

    for (Connection *con : outputConnections) {
        con->display(drawList, node_rect_min, offset, textSize);
    }


    //for (int i = 0; i < node->outputConnections.size(); ++i)
    //	drawList->AddCircleFilled(offset + node->outputSlotPos(i), NODE_SLOT_RADIUS, ImColor(150,150,150,150));

    if (node_moving_active && ImGui::IsMouseDragging(0))
        this->pos = this->pos + ImGui::GetIO().MouseDelta;

    //ImGui::EndGroup();

    ImGui::PopID();
}

bool Node::hasConnection(Connection *connection) {
    for (Connection *con : inputConnections) {
        if (con == connection)
            return true;
    }

    for (Connection *con : outputConnections) {
        if (con == connection)
            return true;
    }
    return false;
}

Connection *Node::getHoverConnection(ImVec2 offset, ImVec2 *pos) {
    ImVec2 nodePos = this->pos + offset;

    for (Connection *con : this->inputConnections) {
        if (con->isHovered(nodePos)) {
            *pos = nodePos + con->getPosition();
            return con;
        }
    }

    for (Connection *con : this->outputConnections) {
        if (con->isHovered(nodePos)) {
            *pos = nodePos + con->getPosition();
            return con;
        }
    }

    return nullptr;
}

std::vector<std::pair<ImVec2, ImVec2>> Node::getLinesToRender() {
    std::vector<std::pair<ImVec2, ImVec2>> fromToPairs;
    for (Connection *con : this->inputConnections) {
        if (con->getInputNode() == nullptr) {
            continue;
        }

        Node *targetNode = con->getInputNode();

        if (!targetNode) {
            continue;
        }
        std::pair<ImVec2, ImVec2> fromTo = std::make_pair(
        targetNode->pos + con->getInput()->getPosition(),
        this->pos + con->getPosition()
        );
        fromToPairs.push_back(fromTo);
    }
    return fromToPairs;
}

bool Node::isHovered(ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    mousePos = mousePos - offset;
    //now check if mousepos is in the Node:
    if((mousePos.x > this->pos.x && mousePos.x < this->pos.x + this->size.x) && //x inside
       (mousePos.y > this->pos.y && mousePos.y < this->pos.y + this->size.y) //y inside
        ) {
        return true;
    }

    for(Connection* con:inputConnections) {
        if(con->isHovered(this->pos + offset)) {
            return true;
        }
    }
    for(Connection* con:outputConnections) {
        if(con->isHovered(this->pos + offset)) {
            return true;
        }
    }
    return false;
}

void Node::addInput(const ConnectionDesc &description) {
        if (description.name.empty()) {
            std::cerr << "Connection creation request failed because of emtpy name" << std::endl;
            return;
        }

        Connection *con = new Connection(this, description, Connection::Directions::INPUT);
        this->inputConnections.push_back(con);
        calculateAndSetDrawInformation();
}

void Node::addOutput(const ConnectionDesc &description) {
    if (description.name.empty()) {
        std::cerr << "Connection creation request failed because of emtpy name" << std::endl;
        return;
    }

    Connection *con = new Connection(this, description, Connection::Directions::OUTPUT);
    this->outputConnections.push_back(con);
    calculateAndSetDrawInformation();
}

Node::~Node() {
    for(Connection* connection:inputConnections) {
        delete connection;
    }
    for(Connection* connection:outputConnections) {
        delete connection;
    }
}
