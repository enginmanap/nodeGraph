//
// Created by engin on 25.03.2019.
//

#include "NodeGraph.h"
#include <algorithm>
#include <sstream>
#include <tinyxml2.h>

void NodeGraph::drawHermite(ImDrawList *drawList, LineDrawInformation drawInformation, int STEPS) {
    ImVec2 t1 = ImVec2(+80.0f, 0.0f);
    ImVec2 t2 = ImVec2(+80.0f, 0.0f);

    for (int step = 0; step <= STEPS; step++) {
        float t = (float) step / (float) STEPS;
        float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
        float h2 = -2 * t * t * t + 3 * t * t;
        float h3 = t * t * t - 2 * t * t + t;
        float h4 = t * t * t - t * t;
        drawList->PathLineTo(ImVec2(h1 * drawInformation.from.x + h2 * drawInformation.to.x + h3 * t1.x + h4 * t2.x, h1 * drawInformation.from.y + h2 * drawInformation.to.y + h3 * t1.y + h4 * t2.y));
    }
    if(drawInformation.connectionHover) {
        drawList->PathStroke(ImColor(100, 100, 100), false, 3.0f);
    } else if(drawInformation.nodeHover) {
        drawList->PathStroke(ImColor(150, 150, 150), false, 3.0f);
    } else {
        drawList->PathStroke(ImColor(200, 200, 100), false, 2.0f);
    }
}

void NodeGraph::display() {
    // 4/5 of window width is main, rest is detail by default
    ImGui::Columns(2, "NodeGraph Main", true);
    if(!isColoumnSizeSet) {
        ImGui::SetColumnWidth(-1, ImGui::GetWindowWidth() - 200);
        isColoumnSizeSet = true;
    }
    ImGui::BeginGroup();
    static ImVec2 scrolling = ImVec2(0.0f, 0.0f);

    // Create our child canvas
    //ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(40, 40, 40, 200));
    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

    ImGui::PushItemWidth(120.0f);

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);
    scrolling.x = -1 *ImGui::GetScrollX();
    scrolling.y = -1 *ImGui::GetScrollY();

    scrolling.x += windowPos.x;
    scrolling.y += windowPos.y;
    //ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

    //displayNode(draw_list, scrolling, s_emittable, selectedNodeID);
    //displayNode(draw_list, scrolling, s_emitter, selectedNodeID);
    for(auto it = nodes.rbegin(); it != nodes.rend(); it++) {
        (*it)->display(draw_list, scrolling, dragNode.con != 0, (*it) == this->selectedNode);
    }

    updateDragging(scrolling);//might fail
    if(!messageList.empty()) {
        ImVec2 position = ImGui::GetWindowPos() + ImVec2(15,15);
        ImVec2 padding = ImVec2(10,10);
        ImU32 color;
        for (auto messageIt = messageList.begin(); messageIt != messageList.end(); ++messageIt) {
            Message message = *messageIt;
            ImVec2 textSize = ImGui::CalcTextSize(message.text.c_str());

            switch (message.type) {
                case MessageTypes::INFO:
                    color = ImColor(0, 200, 0);
                    break;
                case MessageTypes::WARNING:
                    color = ImColor(200, 200, 0);
                    break;
                case MessageTypes::ERROR:
                    color = ImColor(200, 0, 0);
                    break;
            }
            draw_list->AddRectFilled(position - padding, position + textSize + padding, color);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.25, 0.25, 0.25, 1));
            ImGui::SetCursorScreenPos(position);
            ImGui::Text("%s", message.text.c_str());
            ImGui::PopStyleColor();
            position.y +=32;
            //now try to clean up.
            if((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1))&& ( ImGui::GetTime() - message.time > 3.0))  {
                messageIt = messageList.erase(messageIt);
                messageIt--;
            }
        }

    }
    renderLines(draw_list, scrolling);

    draw_list->ChannelsMerge();

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
        setSelectedNodeAndConnection(scrolling);
    }

    // Open context menu
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) {
        setSelectedNodeAndConnection(scrolling);
        state = DisplayStates::MENU_REQUEST;
    }
    if (state == DisplayStates::MENU_REQUEST) {
        ImGui::OpenPopup("context_menu");
        state = DisplayStates::MENU_SHOWN;
    }

    drawContextMenu(selectedNode, scrolling);

    if(state == DisplayStates::RENAME_NODE_REQUEST) {
        ImGui::OpenPopup("changeNameMenu");
        state = DisplayStates::RENAME_NODE;
        //ImGui::SetKeyboardFocusHere(0);
    }

    drawRenameMenu(selectedNode);

    if(state == DisplayStates::ADD_CONNECTION_REQUEST) {
        ImGui::OpenPopup("addConnectionPopup");
        state = DisplayStates::ADD_CONNECTION;
    }

    drawAddConnectionMenu(selectedNode);

    // Scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        scrolling = scrolling - ImGui::GetIO().MouseDelta;

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::EndGroup();
    ImGui::NextColumn();
    ImGui::BeginGroup();
    drawDetailsPane(selectedNode);

    ImGui::EndGroup();
}

void NodeGraph::setSelectedNodeAndConnection(const ImVec2 &scrolling) {
    selectedNode = nullptr;
    hoveredConnection = nullptr;
    for (Node *node:nodes) {
        if (node->isHovered(scrolling)) {
            selectedNode = node;
            if (selectedNode != nodes[0]) {
                auto nodeIt = std::find(nodes.begin(), nodes.end(), selectedNode);
                nodes.erase(nodeIt);
                nodes.insert(nodes.begin(), selectedNode);
            }
            ImVec2 temp;
            hoveredConnection = selectedNode->getHoverConnection(scrolling, &temp);
            break;
        }
    }
}

/**
 * Offset contains scrool + ImGui window position in application.
 * @param selectedNode
 * @param offset
 */
void NodeGraph::drawContextMenu(Node *selectedNode, const ImVec2 &offset) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("context_menu")) {

        if(selectedNode == nullptr ) {
            //Add new node part
            for (size_t i = 0; i < nodeTypes.size(); ++i) {
                if (ImGui::MenuItem(nodeTypes[i]->name.c_str())) {
                    Node *node = new Node(nextNodeID++, ImGui::GetIO().MousePos - offset, nodeTypes[i]);
                    nodes.push_back(node);
                    this->selectedNode = node;
                }
            }
        } else {
            // Selected Node is not null
            if (ImGui::MenuItem("Change Name")) {
                state = DisplayStates::RENAME_NODE_REQUEST;
                strncpy(nodeName, selectedNode->getDisplayName().c_str(), sizeof(nodeName)-1);
            }
            if(ImGui::MenuItem("Remove Node")) {
                auto it = std::find(nodes.begin(), nodes.end(), selectedNode);
                if(it != nodes.end()) {
                    nodes.erase(it);
                    delete selectedNode;
                    this->selectedNode = nullptr;
                }
            } else {
                if (selectedNode->getEditable()) {
                    ImGui::Separator();
                    if (ImGui::MenuItem("Add Input")) {
                        state = DisplayStates::ADD_CONNECTION_REQUEST;
                        connectionRequestType = Connection::Directions::INPUT;
                        strncpy(connectionName, "Input", sizeof(connectionName) - 1);
                    }
                    if (ImGui::MenuItem("Add Output")) {
                        state = DisplayStates::ADD_CONNECTION_REQUEST;
                        connectionRequestType = Connection::Directions::OUTPUT;
                        strncpy(connectionName, "Output", sizeof(connectionName) - 1);
                    }
                    if(hoveredConnection != nullptr) {
                        switch (hoveredConnection->getDirection()) {
                            case Connection::Directions::INPUT: {
                                if (ImGui::MenuItem("Remove Input")) {
                                    selectedNode->removeInput(hoveredConnection);
                                    hoveredConnection = nullptr;
                                }
                            }
                            break;
                            case Connection::Directions::OUTPUT: {
                                if (ImGui::MenuItem("Remove Output")) {
                                    selectedNode->removeOutput(hoveredConnection);
                                    hoveredConnection = nullptr;
                                }
                            }
                            break;
                        }
                    }
                }
                if(hoveredConnection != nullptr) {//if removed check
                    ImGui::Separator();
                    if (ImGui::MenuItem("Clear Connection")) {
                        hoveredConnection->clearConnections();
                    }
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeGraph::drawRenameMenu(Node *selectedNode) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (selectedNode != nullptr && state == DisplayStates::RENAME_NODE && ImGui::BeginPopup("changeNameMenu")) {
        if(ImGui::InputText("New name", nodeName, sizeof(nodeName), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Apply")) {
            selectedNode->setDisplayName(std::string(nodeName));
            state = DisplayStates::NODE_GRAPH;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("Cancel")) {
            state = DisplayStates::NODE_GRAPH;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeGraph::drawAddConnectionMenu(Node *pNode) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (pNode != nullptr && state == DisplayStates::ADD_CONNECTION && ImGui::BeginPopup("addConnectionPopup")) {
        static std::string connectionDataType = *connectionDataTypes.begin();
        for(std::string dataType:connectionDataTypes) {
            if(ImGui::RadioButton(dataType.c_str(), connectionDataType == dataType)) { connectionDataType = dataType;}
        }
        if(ImGui::InputText("Connection name", connectionName, sizeof(connectionName), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Apply")) {
            ConnectionDesc desc;
            desc.type = connectionDataType;
            desc.name = connectionName;
            switch (connectionRequestType ) {
                case Connection::Directions::INPUT: pNode->addInput(desc); break;
                case Connection::Directions::OUTPUT: pNode->addOutput(desc); break;
            }
            state = DisplayStates::NODE_GRAPH;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("Cancel")) {
            state = DisplayStates::NODE_GRAPH;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

Node *NodeGraph::findNodeByCon(Connection *findCon) {
    for (Node *node : nodes) {
        if(node->hasConnection(findCon)) {
            return node;
        }
    }
    return 0;
}

void NodeGraph::renderLines(ImDrawList *drawList, ImVec2 offset) {
    for (Node *node : nodes) {
        std::vector<LineDrawInformation> fromToPairs = node->getLinesToRender(offset);
        for(auto fromTo: fromToPairs) {
            drawHermite(drawList,
                        fromTo,
                        12);
        }
    }
}

Connection *NodeGraph::getHoverCon(ImVec2 offset, ImVec2 *pos) {
    Connection* result = nullptr;
    for (Node *node : nodes) {
        bool barrier = node->isHovered(offset);//Since nodes rendered on reverse order, the ones after first overed are under it, so they should not activate dragging.
        result = node->getHoverConnection(offset, pos);
        if(result != nullptr){
            result->displayDataTooltip();
            break;
        }
        if(barrier) {
            break;
        }
    }
    if(result == nullptr) {
        dragNode.con = 0;
    }
    return result;//return null or found node
}

bool NodeGraph::updateDragging(ImVec2 offset) {
    switch (dragState) {
        case DragState_Default: {
            ImVec2 pos;
            Connection *con = getHoverCon(offset, &pos);

            if (con) {
                dragNode.con = con;
                dragNode.pos = pos;
                dragState = DragState_Hover;
                return true;
            }

            break;
        }

        case DragState_Hover: {
            ImVec2 pos;
            Connection *con = getHoverCon(offset, &pos);

            // Make sure we are still hovering the same node

            if (con != dragNode.con) {
                dragNode.con = 0;
                dragState = DragState_Default;
                return true;
            }

            if (ImGui::IsMouseClicked(0) && dragNode.con) {
                if(dragNode.con->getParent()->getExtension() != nullptr) {
                    if ((dragNode.con->getParent()->getExtension()->isConnectionActive(dragNode.con))) {
                        dragState = DragState_Draging;
                    } else {
                        dragNode.con = 0;
                        addError("This Connection is not draggable.");
                        return false;
                    }
                } else {
                    dragState = DragState_Draging;
                }
            }
            break;
        }

        case DragState_BeginDrag: {
            break;
        }

        case DragState_Draging: {
            ImDrawList *drawList = ImGui::GetWindowDrawList();

            drawList->ChannelsSetCurrent(0); // Background

            LineDrawInformation drawInformation(dragNode.pos, ImGui::GetIO().MousePos, false, false);

            drawHermite(drawList, drawInformation, 12);

            if (!ImGui::IsMouseDown(0)) {
                ImVec2 pos;
                Connection *con = getHoverCon(offset, &pos);

                // Make sure we are still hovering the same node

                if (con == dragNode.con) {
                    dragNode.con = 0;
                    dragState = DragState_Default;
                    return true;
                }

                // Lets connect the nodes.
                /**
                 * we have 2 nodes,
                 * dragNode.con -> started node
                 * con -> ended con.
                 *
                 * we should verify these:
                 * * if one is input, other should be output, and vice versa
                 * * They should be same type (float,vec3 etc.)
                 */

                if((con->getDirection() == Connection::Directions::INPUT && dragNode.con->getDirection() == Connection::Directions::INPUT) ||
                    (con->getDirection() == Connection::Directions::OUTPUT && dragNode.con->getDirection() == Connection::Directions::OUTPUT)) {
                    dragNode.con = 0;
                    dragState = DragState_Default;
                    addError("Dragged to same direction. Match input with output");
                    return false;
                }

                if(con->getDataType() != dragNode.con->getDataType()) {
                    dragNode.con = 0;
                    dragState = DragState_Default;
                    addError("Dragged to different data types");
                    return false;
                }

                Connection* outputSide = nullptr;
                Connection* inputSide = nullptr;
                if(con->getDirection() == Connection::Directions::OUTPUT) {
                    outputSide = con;
                    inputSide = dragNode.con;
                }
                if(dragNode.con->getDirection() == Connection::Directions::OUTPUT) {
                    outputSide = dragNode.con;
                    inputSide = con;
                }
                std::vector<Connection*> backup = inputSide->getInputConnections();
                inputSide->addInputConnection(outputSide);
                dragNode.con = 0;
                dragState = DragState_Default;

                if(isCyclic() && !cycleAllowed) {
                    inputSide->clearConnections();
                    if(!backup.empty()) {
                        for (auto conn:backup) {
                            inputSide->addInputConnection(conn);
                        }
                    }
                    addError("There is a cycle");
                    return false;
                }
            }

            break;
        }

        case DragState_Connect: {
            break;
        }
    }
    return true;
}

bool NodeGraph::depthFirstSearch(Node *currentNode, std::set<Node *>& visitedNodes, std::unordered_map<Node*, bool>& recursionStack) {
    if(visitedNodes.find(currentNode) == visitedNodes.end()) {
        visitedNodes.insert(currentNode);
        recursionStack[currentNode] = true;
        for(Node* child:currentNode->getOutputConnectedNodes()) {
            if(visitedNodes.find(child) == visitedNodes.end() && depthFirstSearch(child, visitedNodes, recursionStack)) {
                return true;
            } else if (recursionStack.find(child) != recursionStack.end() && recursionStack[child]) {
                return true;
            }
        }
    }
    recursionStack[currentNode] = false;
    return false;
}

bool NodeGraph::isCyclic() {
    std::set<Node*> visitedNodes;
    std::unordered_map<Node*, bool> recursionStack;
    for(Node* node:nodes) {
        if(depthFirstSearch(node, visitedNodes, recursionStack)) {
            return true;
        }
    }
    return false;
}

void NodeGraph::drawDetailsPane(Node* selectedNode) {
    if(editorExtension != nullptr) {
        if(ImGui::CollapsingHeader("Graph Details", ImGuiTreeNodeFlags_DefaultOpen)) {
            std::vector<const Node*> tempVector;
            for (size_t i = 0; i < this->nodes.size(); ++i) {
                tempVector.push_back(nodes[i]);
            }
            editorExtension->drawDetailPane(this, tempVector, this->selectedNode);
        }
    }
    if(selectedNode != nullptr && selectedNode->getExtension() != nullptr){
        if(ImGui::CollapsingHeader("Node Details", ImGuiTreeNodeFlags_DefaultOpen)) {
            selectedNode->getExtension()->drawDetailPane(selectedNode);
        }
    }
}

void NodeGraph::serialize(const std::string& fileName) {
    tinyxml2::XMLDocument serializeDocument;
    tinyxml2::XMLElement * rootNode = serializeDocument.NewElement("NodeGraph");
    serializeDocument.InsertFirstChild(rootNode);

    if(editorExtension != nullptr) {
        this->editorExtension->serialize(serializeDocument, rootNode);
    }

    tinyxml2::XMLElement * nodeTypesElement = serializeDocument.NewElement("NodeTypes");
    rootNode->InsertEndChild(nodeTypesElement);

    for (size_t i = 0; i < nodeTypes.size(); ++i) {
        nodeTypes[i]->serialize(serializeDocument, nodeTypesElement);
    }

    tinyxml2::XMLElement * nodesElement = serializeDocument.NewElement("Nodes");
    rootNode->InsertEndChild(nodesElement);

    for (size_t i = 0; i < nodes.size(); ++i) {
        nodes[i]->serialize(serializeDocument, nodesElement);
    }

    tinyxml2::XMLError eResult = serializeDocument.SaveFile(fileName.c_str());
    if(eResult != tinyxml2::XML_SUCCESS) {
        std::cerr  << "ERROR " << eResult << std::endl;
    }
}

NodeGraph * NodeGraph::deserialize(const std::string& fileName,
        std::unordered_map<std::string, std::function<EditorExtension*()>> possibleEditorExtensions,
        std::unordered_map<std::string, std::function<NodeExtension*(const NodeType*)>> possibleNodeExtensions) {
    tinyxml2::XMLDocument xmlDoc;
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML "<< fileName << ": " <<  xmlDoc.ErrorName() << std::endl;
        return nullptr;
    }

    tinyxml2::XMLNode * rootNode = xmlDoc.FirstChild();
    if (rootNode == nullptr) {
        std::cerr << "World xml is not a valid XML." << std::endl;
        return nullptr;
    }

    EditorExtension* usedEditorExtension = nullptr;
    tinyxml2::XMLElement* editorExtensionElement =  rootNode->FirstChildElement("EditorExtension");
    if (editorExtensionElement == nullptr) {
        std::cerr << "Error loading XML " << fileName << ": EditorExtension is not found!" << std::endl;
    } else {
        tinyxml2::XMLElement* editorExtensionNameElement =  editorExtensionElement->FirstChildElement("Name");
        if (editorExtensionNameElement == nullptr) {
            std::cerr << "Error loading XML " << fileName << ": EditorExtension has no name!" << std::endl;
        } else {
            if(editorExtensionNameElement->GetText() == nullptr) {
                std::cerr << "Error loading XML "<< fileName << ": EditorExtension name has no text!" << std::endl;
            } else {
                if(possibleEditorExtensions.find(editorExtensionNameElement->GetText()) != possibleEditorExtensions.end()) {
                    usedEditorExtension = possibleEditorExtensions[editorExtensionNameElement->GetText()]();
                    usedEditorExtension->deserialize(fileName, editorExtensionElement);
                }
            }
        }

    }

    tinyxml2::XMLElement* nodeTypesElement =  rootNode->FirstChildElement("NodeTypes");
    if (nodeTypesElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": NodeTypes are not found!" << std::endl;
        return nullptr;
    }
    tinyxml2::XMLElement* nodeTypeElement =  nodeTypesElement->FirstChildElement("NodeType");
    if (nodeTypeElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": Not even one NodeType found!" << std::endl;
        return nullptr;
    }
    std::vector<NodeType*> nodeTypes;

    while(nodeTypeElement != nullptr) {
        NodeType* nodeType = NodeType::deserialize(fileName, nodeTypeElement, possibleNodeExtensions);
        if(nodeType == nullptr) {
            return nullptr;
        }
        nodeTypes.emplace_back(nodeType);
        nodeTypeElement = nodeTypeElement->NextSiblingElement("NodeType");

    }

    tinyxml2::XMLElement* nodesElement =  rootNode->FirstChildElement("Nodes");
    if (nodesElement == nullptr) {
        std::cerr << "Error loading XML "<< fileName << ": No Nodes found!" << std::endl;
        return nullptr;
    }
    NodeGraph* newNodeGraph = new NodeGraph(nodeTypes, false, usedEditorExtension);
    tinyxml2::XMLElement* nodeElement =  nodesElement->FirstChildElement("Node");
    std::unordered_map<Node*, std::unordered_map<Connection*, std::vector<LateDeserializeInformation>>> lateSerializeInputsPerNode;
    std::unordered_map<Node*, std::unordered_map<Connection*, std::vector<LateDeserializeInformation>>> lateSerializeOutputsPerNode;
    while(nodeElement != nullptr) {
        std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> lateDeserializeInputs;
        std::unordered_map<Connection*, std::vector<LateDeserializeInformation>> lateDeserializeOutputs;
        Node* node = Node::deserialize(fileName, nodeElement, nodeTypes, lateDeserializeInputs, lateDeserializeOutputs);
        if(node == nullptr) {
            return nullptr;
        }
        lateSerializeInputsPerNode[node] = lateDeserializeInputs;
        lateSerializeOutputsPerNode[node] = lateDeserializeOutputs;
        newNodeGraph->nodes.emplace_back(node);
        newNodeGraph->nextNodeID = std::max(node->getId() +1, newNodeGraph->nextNodeID);
        nodeElement =  nodeElement->NextSiblingElement("Node");
    }

    //now deserialize late part
    for(auto node:newNodeGraph->nodes) {
        node->lateDeserialize(lateSerializeInputsPerNode[node], lateSerializeOutputsPerNode[node], newNodeGraph->nodes);
    }
    return newNodeGraph;
}