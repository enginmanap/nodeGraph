//
// Created by engin on 25.03.2019.
//

#include "Connection.h"
#include "Node.h"

bool Connection::isHovered(ImVec2 offset) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    ImVec2 conPos = offset + this->pos;

    float xd = mousePos.x - conPos.x;
    float yd = mousePos.y - conPos.y;

    return ((xd * xd) + (yd * yd)) < (NODE_SLOT_RADIUS * NODE_SLOT_RADIUS);
}

/**
 * Display should get information about the offset from Node,
 * and it should provide offset.
 *
 * This is because text of the connection is not known, so size needs calculation.
 */
void Connection::display(ImDrawList *drawList, const ImVec2 node_rect_min, ImVec2 &offset, ImVec2 &textSize) {
    switch (type) {
        case Types::INPUT: {
            ImGui::SetCursorScreenPos(offset + ImVec2(10.0f, 0));
            ImGui::Text("%s", this->desc.name.c_str());

            ImColor conColor = ImColor(150, 150, 150);

            if (this->isHovered(node_rect_min))
                conColor = ImColor(200, 200, 200);

            drawList->AddCircleFilled(node_rect_min + this->pos, NODE_SLOT_RADIUS, conColor);

            offset.y += textSize.y + 2.0f;
        }
        break;
        case Types::OUTPUT: {
            textSize = ImGui::CalcTextSize(this->desc.name.c_str());

            ImGui::SetCursorScreenPos(offset + ImVec2(this->pos.x - (textSize.x + 10.0f), 0));
            ImGui::Text("%s", this->desc.name.c_str());

            ImColor conColor = ImColor(150, 150, 150);

            if (this->isHovered(node_rect_min))
                conColor = ImColor(200, 200, 200);

            drawList->AddCircleFilled(node_rect_min + this->pos, NODE_SLOT_RADIUS, conColor);

            offset.y += textSize.y + 2.0f;
        }
        break;
    }
    if(this->isHovered(node_rect_min)) {
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
            ImGui::Text(" %s ",this->desc.getTypeString().c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

void Connection::setPosition(float TitleSizeY, float& textSizeY, float xPosition) {
    ImVec2 textSize = this->getTextSize();

    this->pos = ImVec2(xPosition, TitleSizeY + textSizeY + textSize.y / 2.0f);

    textSizeY += textSize.y;
    textSizeY += 4.0f;        // size between text entries
}

