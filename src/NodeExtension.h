//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_NODEEXTENSION_H
#define NODEGRAPH_NODEEXTENSION_H

class Node;
class Connection;

class NodeExtension {
public:
    virtual void drawDetailPane(Node* node) = 0;

    virtual ~NodeExtension() {}

    virtual bool isConnectionActive(Connection* connection [[gnu::unused]]) { return true;}

};


#endif //NODEGRAPH_NODEEXTENSION_H
