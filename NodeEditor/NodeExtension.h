//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_NODEEXTENSION_H
#define NODEGRAPH_NODEEXTENSION_H

class Node;

class NodeExtension {
public:
    virtual void drawDetailPane(Node* node) = 0;

};


#endif //NODEGRAPH_NODEEXTENSION_H
