// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <stdint.h>

#include "NodeEditor/Node.h"
#include "NodeEditor/Connection.h"
#include "NodeEditor/DragNode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}
/*
static void saveNodes(const char* filename)
{
    json_t* root = json_object();
    json_t* nodes = json_array();
    for (Node* node : s_nodes)
	{
		json_t* item = json_object();
		json_object_set_new(item, "type", json_string(node->name));
		json_object_set_new(item, "id", json_integer(node->id));
		json_object_set_new(item, "pos", json_pack("{s:f, s:f}", "x",  node->pos.x, "y", node->pos.y));
		json_array_append_new(nodes, item);
	}
    // save the nodes
    json_object_set_new(root, "nodes", nodes);
    if (json_dump_file(root, filename, JSON_INDENT(4) | JSON_PRESERVE_ORDER) != 0)
        printf("JSON: Unable to open %s for write\n", filename);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawHermite(ImDrawList* drawList, ImVec2 p1, ImVec2 p2, int STEPS)
{
    ImVec2 t1 = ImVec2(+80.0f, 0.0f);
    ImVec2 t2 = ImVec2(+80.0f, 0.0f);

    for (int step = 0; step <= STEPS; step++)
    {
        float t = (float)step / (float)STEPS;
        float h1 = +2*t*t*t - 3*t*t + 1.0f;
        float h2 = -2*t*t*t + 3*t*t;
        float h3 =    t*t*t - 2*t*t + t;
        float h4 =    t*t*t -   t*t;
        drawList->PathLineTo(ImVec2(h1*p1.x + h2*p2.x + h3*t1.x + h4*t2.x, h1*p1.y + h2*p2.y + h3*t1.y + h4*t2.y));
    }

    drawList->PathStroke(ImColor(200,200,100), false, 3.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void updateDraging(ImVec2 offset)
{
    switch (s_dragState)
    {
        case DragState_Default:
        {
            ImVec2 pos;
            Connection* con = getHoverCon(offset, &pos);

            if (con)
            {
                s_dragNode.con = con;
                s_dragNode.pos = pos;
                s_dragState = DragState_Hover;
                return;
            }

            break;
        }

        case DragState_Hover:
        {
            ImVec2 pos;
            Connection* con = getHoverCon(offset, &pos);

            // Make sure we are still hovering the same node

            if (con != s_dragNode.con)
            {
                s_dragNode.con = 0;
                s_dragState = DragState_Default;
                return;
            }

            if (ImGui::IsMouseClicked(0) && s_dragNode.con)
                s_dragState = DragState_Draging;

            break;
        }

        case DragState_BeginDrag:
        {
            break;
        }

        case DragState_Draging:
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            drawList->ChannelsSetCurrent(0); // Background

            drawHermite(drawList, s_dragNode.pos, ImGui::GetIO().MousePos, 12);

            if (!ImGui::IsMouseDown(0))
            {
                ImVec2 pos;
                Connection* con = getHoverCon(offset, &pos);

                // Make sure we are still hovering the same node

                if (con == s_dragNode.con)
                {
                    s_dragNode.con = 0;
                    s_dragState = DragState_Default;
                    return;
                }

                // Lets connect the nodes.
                // TODO: Make sure we connect stuff in the correct way!

                con->input = s_dragNode.con;
                s_dragNode.con = 0;
                s_dragState = DragState_Default;
            }

            break;
        }

        case DragState_Connect:
        {
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void displayNode(ImDrawList* drawList, ImVec2 offset, Node* node, int& node_selected)
{
    int node_hovered_in_scene = -1;
    bool open_context_menu = false;

    ImGui::PushID(node->id);
    ImVec2 node_rect_min = offset + node->pos;

    // Display node contents first
    drawList->ChannelsSetCurrent(1); // Foreground
    bool old_any_active = ImGui::IsAnyItemActive();

    // Draw title in center

    ImVec2 textSize = ImGui::CalcTextSize(node->name);

    ImVec2 pos = node_rect_min + NODE_WINDOW_PADDING;
    pos.x = node_rect_min.x + (node->size.x / 2) - textSize.x / 2;

    ImGui::SetCursorScreenPos(pos);
    //ImGui::BeginGroup(); // Lock horizontal position
    ImGui::Text("%s", node->name);
    //ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
    //float dummy_color[3] = { node->Pos.x / ImGui::GetWindowWidth(), node->Pos.y / ImGui::GetWindowHeight(), fmodf((float)node->ID * 0.5f, 1.0f) };
    //ImGui::ColorEdit3("##color", &dummy_color[0]);

    // Save the size of what we have emitted and weither any of the widgets are being used
    bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
    //node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
    ImVec2 node_rect_max = node_rect_min + node->size;

    // Display node box
    drawList->ChannelsSetCurrent(0); // Background

    ImGui::SetCursorScreenPos(node_rect_min);
    ImGui::InvisibleButton("node", node->size);

    if (ImGui::IsItemHovered())
    {
        node_hovered_in_scene = node->id;
        open_context_menu |= ImGui::IsMouseClicked(1);
    }

    bool node_moving_active = false;

    if (ImGui::IsItemActive() && !s_dragNode.con)
        node_moving_active = true;

    ImU32 node_bg_color = node_hovered_in_scene == node->id ? ImColor(75,75,75) : ImColor(60,60,60);
    drawList->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);

    ImVec2 titleArea = node_rect_max;
    titleArea.y = node_rect_min.y + 30.0f;

    // Draw text bg area
    drawList->AddRectFilled(node_rect_min + ImVec2(1,1), titleArea, ImColor(100,0,0), 4.0f);
    drawList->AddRect(node_rect_min, node_rect_max, ImColor(100,100,100), 4.0f);

    ImVec2 off;

    offset.y += 40.0f;

    offset = offset + node_rect_min;

    off.x = node_rect_min.x;
    off.y = node_rect_min.y;

    for (Connection* con : node->inputConnections)
    {
        ImGui::SetCursorScreenPos(offset + ImVec2(10.0f, 0));
        ImGui::Text("%s", con->desc.name);

        ImColor conColor = ImColor(150, 150, 150);

        if (isConnectorHovered(con, node_rect_min))
            conColor = ImColor(200, 200, 200);

        drawList->AddCircleFilled(node_rect_min + con->pos, NODE_SLOT_RADIUS, conColor);

        offset.y += textSize.y + 2.0f;
    }

    offset = node_rect_min;
    offset.y += 40.0f;

    for (Connection* con : node->outputConnections)
    {
        textSize = ImGui::CalcTextSize(con->desc.name);

        ImGui::SetCursorScreenPos(offset + ImVec2(con->pos.x - (textSize.x + 10.0f), 0));
        ImGui::Text("%s", con->desc.name);

        ImColor conColor = ImColor(150, 150, 150);

        if (isConnectorHovered(con, node_rect_min))
            conColor = ImColor(200, 200, 200);

        drawList->AddCircleFilled(node_rect_min + con->pos, NODE_SLOT_RADIUS, conColor);

        offset.y += textSize.y + 2.0f;
    }


    //for (int i = 0; i < node->outputConnections.size(); ++i)
    //	drawList->AddCircleFilled(offset + node->outputSlotPos(i), NODE_SLOT_RADIUS, ImColor(150,150,150,150));

    if (node_widgets_active || node_moving_active)
        node_selected = node->id;

    if (node_moving_active && ImGui::IsMouseDragging(0))
        node->pos = node->pos + ImGui::GetIO().MouseDelta;

    //ImGui::EndGroup();

    ImGui::PopID();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void renderLines(ImDrawList* drawList, ImVec2 offset)
{
    for (Node* node : s_nodes)
    {
        for (Connection* con : node->inputConnections)
        {
            if (!con->input)
                continue;

            Node* targetNode = findNodeByCon(con->input);

            if (!targetNode)
                continue;

            drawHermite(drawList,
                        offset + targetNode->pos + con->input->pos,
                        offset + node->pos + con->pos,
                        12);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ShowExampleAppCustomNodeGraph(bool* opened)
{
    ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Example: Custom Node Graph", opened))
    {
        ImGui::End();
        return;
    }

    bool open_context_menu = false;
    int node_hovered_in_list = -1;
    int node_hovered_in_scene = -1;

    static int node_selected = -1;
    static ImVec2 scrolling = ImVec2(0.0f, 0.0f);

    ImGui::SameLine();
    ImGui::BeginGroup();

    // Create our child canvas
    //ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1,1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(40,40,40,200));
    ImGui::BeginChild("scrolling_region", ImVec2(0,0), true, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove);
    ImGui::PushItemWidth(120.0f);


    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSplit(2);
    //ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

    //displayNode(draw_list, scrolling, s_emittable, node_selected);
    //displayNode(draw_list, scrolling, s_emitter, node_selected);

    for (Node* node : s_nodes)
        displayNode(draw_list, scrolling, node, node_selected);

    updateDraging(scrolling);
    renderLines(draw_list, scrolling);

    draw_list->ChannelsMerge();

    // Open context menu
    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
    {
        node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
        open_context_menu = true;
    }
    if (open_context_menu)
    {
        ImGui::OpenPopup("context_menu");
        if (node_hovered_in_list != -1)
            node_selected = node_hovered_in_list;
        if (node_hovered_in_scene != -1)
            node_selected = node_hovered_in_scene;
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
    if (ImGui::BeginPopup("context_menu"))
    {
        if (ImGui::MenuItem("Load graph..."))
        {
            /*
            char path[1024];
            if (Dialog_open(path))
            {
                printf("file to load %s\n", path);
            }
            */
        }

        if (ImGui::MenuItem("Save graph..."))
        {
            /*
            char path[1024];
            if (Dialog_save(path))
            {
                saveNodes(path);
            }
            */
        }


        /*
        Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (node)
        {
            ImGui::Text("Node '%s'", node->Name);
            ImGui::Separator();
            if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
            if (ImGui::MenuItem("Delete", NULL, false, false)) {}
            if (ImGui::MenuItem("Copy", NULL, false, false)) {}
        }
        */
        //else

        for (int i = 0; i < (int)sizeof_array(s_nodeTypes); ++i)
        {
            if (ImGui::MenuItem(s_nodeTypes[i].name))
            {
                Node* node = createNodeFromType(ImGui::GetIO().MousePos, &s_nodeTypes[i]);
                s_nodes.push_back(node);
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    // Scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        scrolling = scrolling - ImGui::GetIO().MouseDelta;

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::EndGroup();

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int mainOld(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    // Setup ImGui binding

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    /*
    s_emitter = createNodeFromName(ImVec2(500.0f, 100.0f), "Emitter");
    s_emittable = createNodeFromName(ImVec2(500.0f, 300.0f), "Emittable");
    s_quad = createNodeFromName(ImVec2(500.0f, 600.0f), "Quad");
    */

    bool show_test_window = true;
    //bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        ImGuiIO& io = ImGui::GetIO();
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();

        ShowExampleAppCustomNodeGraph(&show_test_window);

        // Rendering
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
/*

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

*/
        bool show_test_window = true;
        ShowExampleAppCustomNodeGraph(&show_test_window);



        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}