//
// Created by engin on 25.03.2019.
//

#include "NodeGraph.h"

void NodeGraph::display() {
    bool open_context_menu = false;
    int node_hovered_in_list = -1;
    int node_hovered_in_scene = -1;

    static int node_selected = -1;
    static ImVec2 scrolling = ImVec2(0.0f, 0.0f);

    // Create our child canvas
    //ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(40, 40, 40, 200));
    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
    ImGui::PushItemWidth(120.0f);


    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);
    //ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

    //displayNode(draw_list, scrolling, s_emittable, node_selected);
    //displayNode(draw_list, scrolling, s_emitter, node_selected);

    for (Node *node : s_nodes)
        node->display(draw_list, scrolling, node_selected);

    updateDragging(scrolling);
    renderLines(draw_list, scrolling);

    draw_list->ChannelsMerge();

    // Open context menu
    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1)) {
        node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
        open_context_menu = true;
    }
    if (open_context_menu) {
        ImGui::OpenPopup("context_menu");
        if (node_hovered_in_list != -1)
            node_selected = node_hovered_in_list;
        if (node_hovered_in_scene != -1)
            node_selected = node_hovered_in_scene;
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("context_menu")) {
        if (ImGui::MenuItem("Load graph...")) {
            /*
            char path[1024];
            if (Dialog_open(path))
            {
                printf("file to load %s\n", path);
            }
            */
        }

        if (ImGui::MenuItem("Save graph...")) {
            /*
            char path[1024];
            if (Dialog_save(path))
            {
                saveNodes(path);
            }
            */
        }


        /*
        Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (node)
        {
            ImGui::Text("Node '%s'", node->Name);
            ImGui::Separator();
            if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
            if (ImGui::MenuItem("Delete", NULL, false, false)) {}
            if (ImGui::MenuItem("Copy", NULL, false, false)) {}
        }
        */
        //else

        for (int i = 0; i < (int) sizeof_array(s_nodeTypes); ++i) {
            if (ImGui::MenuItem(s_nodeTypes[i].name)) {
                Node *node = new Node(ImGui::GetIO().MousePos, &s_nodeTypes[i]);
                s_nodes.push_back(node);
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    // Scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        scrolling = scrolling - ImGui::GetIO().MouseDelta;

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

Node *NodeGraph::findNodeByCon(Connection *findCon) {
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


void NodeGraph::renderLines(ImDrawList *drawList, ImVec2 offset) {
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

Connection *NodeGraph::getHoverCon(ImVec2 offset, ImVec2 *pos) {
    for (Node *node : s_nodes) {
        ImVec2 nodePos = node->pos + offset;

        for (Connection *con : node->inputConnections) {
            if (con->isHovered(nodePos)) {
                *pos = nodePos + con->pos;
                return con;
            }
        }

        for (Connection *con : node->outputConnections) {
            if (con->isHovered(nodePos)) {
                *pos = nodePos + con->pos;
                return con;
            }
        }
    }

    s_dragNode.con = 0;
    return 0;
}

void NodeGraph::updateDragging(ImVec2 offset) {
    switch (s_dragState) {
        case DragState_Default: {
            ImVec2 pos;
            Connection *con = getHoverCon(offset, &pos);

            if (con) {
                s_dragNode.con = con;
                s_dragNode.pos = pos;
                s_dragState = DragState_Hover;
                return;
            }

            break;
        }

        case DragState_Hover: {
            ImVec2 pos;
            Connection *con = getHoverCon(offset, &pos);

            // Make sure we are still hovering the same node

            if (con != s_dragNode.con) {
                s_dragNode.con = 0;
                s_dragState = DragState_Default;
                return;
            }

            if (ImGui::IsMouseClicked(0) && s_dragNode.con)
                s_dragState = DragState_Draging;

            break;
        }

        case DragState_BeginDrag: {
            break;
        }

        case DragState_Draging: {
            ImDrawList *drawList = ImGui::GetWindowDrawList();

            drawList->ChannelsSetCurrent(0); // Background

            drawHermite(drawList, s_dragNode.pos, ImGui::GetIO().MousePos, 12);

            if (!ImGui::IsMouseDown(0)) {
                ImVec2 pos;
                Connection *con = getHoverCon(offset, &pos);

                // Make sure we are still hovering the same node

                if (con == s_dragNode.con) {
                    s_dragNode.con = 0;
                    s_dragState = DragState_Default;
                    return;
                }

                // Lets connect the nodes.
                // TODO: Make sure we connect stuff in the correct way!

                con->input = s_dragNode.con;
                s_dragNode.con = 0;
                s_dragState = DragState_Default;
            }

            break;
        }

        case DragState_Connect: {
            break;
        }
    }
}