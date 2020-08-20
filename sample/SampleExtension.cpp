//
// Created by engin on 6.04.2019.
//

#include "SampleExtension.h"
#include "ImGui/imgui.h"
#include "../src/Connection.h"

void SampleExtension::drawDetailPane(Node *node) {
    ImGui::Text("Extension Text");
    if (ImGui::BeginCombo("Dropdown selection", (type == PossibleTypes::NONE ? "No type Selected." : getNameOfType(type).c_str()))) {
        std::vector<PossibleTypes> allPossibleTypes = getAllValidTypes();
        for (auto currentType = allPossibleTypes.begin(); currentType != allPossibleTypes.end(); ++currentType) {
            if (ImGui::Selectable(getNameOfType(*currentType).c_str(), type == *currentType)) {
                type = *currentType;
            }
            if (type == *currentType) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

bool SampleExtension::isConnectionActive(Connection* connection) {
    if(connection->getName() == "disabled") {
        return false;
    }
    return true;
}