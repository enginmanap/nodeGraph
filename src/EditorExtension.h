//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_EDITOREXTENSION_H
#define NODEGRAPH_EDITOREXTENSION_H

#include <vector>

class Node;

class EditorExtension {
public:
    virtual void drawDetailPane(const std::vector<const Node *>& nodes, const Node* selectedNode = nullptr) = 0;

    virtual ~EditorExtension() {}
};


#endif //NODEGRAPH_EDITOREXTENSION_H
