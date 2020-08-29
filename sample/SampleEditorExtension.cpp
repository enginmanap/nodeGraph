//
// Created by engin on 6.04.2019.
//

#include "SampleEditorExtension.h"
#include "ImGui/imgui.h"
#include "../src/NodeGraph.h"


void SampleEditorExtension::drawDetailPane(NodeGraph* nodeGraph, const std::vector<const Node *>& nodes, const Node* selectedNode) {
    ImGui::Text("Graph Detail Extension");

    ImGui::InputText("custom input:", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_CharsNoBlank);

    if (ImGui::Button("Save Nodes")) {
        if(std::strlen(this->inputBuffer) > 0) {
            nodeGraph->serialize(std::string(this->inputBuffer));
            //this->errorGenerated = true;
            //errorGenerationTime = ImGui::GetTime();
            //this->errorMessage = "Save done.";
        }
    }
}
