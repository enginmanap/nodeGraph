//
// Created by engin on 25.03.2019.
//

#include "DragNode.h"

DragNode s_dragNode;
DragState s_dragState = DragState_Default;

void updateDraging(ImVec2 offset)
{
    switch (s_dragState)
    {
        case DragState_Default:
        {
            ImVec2 pos;
            Connection* con = getHoverCon(offset, &pos);

            if (con)
            {
                s_dragNode.con = con;
                s_dragNode.pos = pos;
                s_dragState = DragState_Hover;
                return;
            }

            break;
        }

        case DragState_Hover:
        {
            ImVec2 pos;
            Connection* con = getHoverCon(offset, &pos);

            // Make sure we are still hovering the same node

            if (con != s_dragNode.con)
            {
                s_dragNode.con = 0;
                s_dragState = DragState_Default;
                return;
            }

            if (ImGui::IsMouseClicked(0) && s_dragNode.con)
                s_dragState = DragState_Draging;

            break;
        }

        case DragState_BeginDrag:
        {
            break;
        }

        case DragState_Draging:
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            drawList->ChannelsSetCurrent(0); // Background

            drawHermite(drawList, s_dragNode.pos, ImGui::GetIO().MousePos, 12);

            if (!ImGui::IsMouseDown(0))
            {
                ImVec2 pos;
                Connection* con = getHoverCon(offset, &pos);

                // Make sure we are still hovering the same node

                if (con == s_dragNode.con)
                {
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

        case DragState_Connect:
        {
            break;
        }
    }
}