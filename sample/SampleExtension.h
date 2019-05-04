//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_SAMPLEEXTENSION_H
#define NODEGRAPH_SAMPLEEXTENSION_H


#include "../src/NodeExtension.h"

class SampleExtension : public NodeExtension {
public:
    void drawDetailPane(Node *node) override;

    bool isConnectionActive(Connection* connection);

};


#endif //NODEGRAPH_SAMPLEEXTENSION_H
