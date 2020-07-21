//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_SAMPLEEDITOREXTENSION_H
#define NODEGRAPH_SAMPLEEDITOREXTENSION_H


#include <string>
#include "../src/EditorExtension.h"

class SampleEditorExtension : public EditorExtension {
public:
    void drawDetailPane(const std::vector<const Node *>& nodes, const Node* selectedNode = nullptr) override;

    std::string getName() {
        return "SampleEditorExtension";
    }
};


#endif //NODEGRAPH_SAMPLEEDITOREXTENSION_H
