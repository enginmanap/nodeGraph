//
// Created by engin on 25.03.2019.
//

#ifndef NODEGRAPH_NODEGRAPH_H
#define NODEGRAPH_NODEGRAPH_H


#include <list>
#include "Node.h"
#include "EditorExtension.h"

class NodeGraph {
    enum class DisplayStates { NODE_GRAPH,
            MENU_REQUEST, MENU_SHOWN,
            RENAME_NODE_REQUEST, RENAME_NODE,
            ADD_CONNECTION_REQUEST, ADD_CONNECTION, };

    DisplayStates state = DisplayStates::NODE_GRAPH;
    uint32_t nextNodeID = 0;

    enum DragState {
        DragState_Default,
        DragState_Hover,
        DragState_BeginDrag,
        DragState_Draging,
        DragState_Connect,
    };

    struct DragNode {
        ImVec2 pos;
        Connection *con;
    };

    enum class MessageTypes {INFO, WARNING, ERROR};

    struct Message {
        std::string text;
        MessageTypes type;
        double time;
    };

    std::vector<NodeType*> nodeTypes;
    std::set<std::string> connectionDataTypes;
    std::vector<Node *> nodes;//first node is rendered last, and it is at the top.
    Node* selectedNode = nullptr;
    Connection* hoveredConnection = nullptr;
    Connection::Directions connectionRequestType = Connection::Directions::INPUT;
    bool cycleAllowed = true;
    EditorExtension* editorExtension = nullptr;
    char nodeName[128] = {0};
    char connectionName[128] = {0};

    std::list<Message> messageList;

    DragState dragState = DragState_Default;
    DragNode dragNode;

    void drawHermite(ImDrawList *drawList, ImVec2 p1, ImVec2 p2, int STEPS);
    bool depthFirstSearch(Node *root, Node *search, std::set<Node *> visitedNodes, bool &cycle);
    void drawContextMenu(Node *selectedNode, const ImVec2 &offset);
    void drawRenameMenu(Node *selectedNode);
    void drawAddConnectionMenu(Node *pNode);
    void drawDetailsPane(Node* selectedNode);
    Node *findNodeByCon(Connection *findCon);
    void renderLines(ImDrawList *drawList, ImVec2 offset);
    void setSelectedNodeAndConnection(const ImVec2 &scrolling);

    Connection *getHoverCon(ImVec2 offset, ImVec2 *pos);

    bool updateDragging(ImVec2 offset);
    bool isCyclic();

public:

    explicit NodeGraph(std::vector<NodeType*> nodeTypes, bool cycleAllowed = true, EditorExtension* editorExtension = nullptr) : nodeTypes(nodeTypes), cycleAllowed(cycleAllowed),
    editorExtension(editorExtension) {
        for(NodeType* nodeType:nodeTypes) {
            for(ConnectionDesc connectionDesc:nodeType->inputConnections) {
                connectionDataTypes.insert(connectionDesc.type);
            }
            for(ConnectionDesc connectionDesc:nodeType->outputConnections) {
                connectionDataTypes.insert(connectionDesc.type);
            }
        }
    };
    void display();

    static NodeGraph * deserialize(const std::string& fileName,
            std::unordered_map<std::string, std::function<EditorExtension*()>> possibleEditorExtensions,
            std::unordered_map<std::string, std::function<NodeExtension*()>> possibleNodeExtensions);

    void serialize(const std::string& fileName);

    void addMessage(const std::string& text) {
        Message newMessage;
        newMessage.text = text;
        newMessage.type = MessageTypes::INFO;
        newMessage.time = ImGui::GetTime();
        messageList.emplace_back(newMessage);
    }

    void addError(const std::string& text) {
        Message newMessage;
        newMessage.text = text;
        newMessage.type = MessageTypes::ERROR;
        newMessage.time = ImGui::GetTime();
        messageList.emplace_back(newMessage);
    }
};


#endif //NODEGRAPH_NODEGRAPH_H
