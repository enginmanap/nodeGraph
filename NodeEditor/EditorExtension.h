//
// Created by engin on 6.04.2019.
//

#ifndef NODEGRAPH_EDITOREXTENSION_H
#define NODEGRAPH_EDITOREXTENSION_H


class EditorExtension {
public:
    virtual void drawDetailPane() = 0;

    virtual ~EditorExtension() {}
};


#endif //NODEGRAPH_EDITOREXTENSION_H
