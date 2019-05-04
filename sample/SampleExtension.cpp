//
// Created by engin on 6.04.2019.
//

#include "SampleExtension.h"
#include "ImGui/imgui.h"
#include "../src/Connection.h"

void SampleExtension::drawDetailPane(Node *node) {
    ImGui::Text("Extension Text");
}

bool SampleExtension::isConnectionActive(Connection* connection) {
    if(connection->getName() == "disabled") {
        return false;
    }
    return true;
}