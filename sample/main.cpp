// ImGui - standalone example application for Glfw + OpenGL 3, using programmable pipeline

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "../src/Node.h"
#include "../src/NodeGraph.h"
#include "SampleExtension.h"
#include "SampleEditorExtension.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
static struct NodeType nodeTypes[] =
        {
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Math
                {
                        "Texture",
                        false,
                        // Input connections
                        {
                                {"Write", ConnectionType_Vec3},
                        },
                        // Output
                        {
                                {"Read", ConnectionType_Vec3},
                        },
                },
                {
                        "PointShadows",
                        false,
                        // Input connections
                        {
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Vec3},
                        },
                },

                {
                        "WorldRender",
                        false,
                        // Input connections
                        {
                                {"Point Shadows", ConnectionType_Vec3},
                                {"Directional Shadows", ConnectionType_Vec3},
                        },
                        // Output
                        {
                                {"Diffuse and Specular", ConnectionType_Vec3},
                                {"Ambient", ConnectionType_Vec3},
                                {"Normal", ConnectionType_Vec3},
                                {"Depth", ConnectionType_Vec3},
                        },
                },
                {
                        "SSAO Generation",
                        false,
                        // Input connections
                        {
                                {"Noise", ConnectionType_Vec3},
                                {"Normal", ConnectionType_Vec3},
                                {"Depth", ConnectionType_Vec3},
                        },
                        // Output
                        {
                                {"SSAO", ConnectionType_Vec3},
                        },
                },

                {
                        "SSAO Blur",
                        false,
                        // Input connections
                        {
                                {"SSAO", ConnectionType_Vec3},
                        },
                        // Output
                        {
                                {"Blurred SSAO", ConnectionType_Vec3},
                        },
                },

                {
                        "Combine",
                        false,
                        // Input connections
                        {
                                {"Diffuse and Specular", ConnectionType_Vec3},
                                {"Ambient", ConnectionType_Vec3},
                                {"SSAO Blurred", ConnectionType_Vec3},
                                {"Depth", ConnectionType_Vec3},
                        },
                        // Output
                        {
                        },
                },



                {
                        "Multiply",
                        true,
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Float},
                        },
                },

                {
                        "Add",
                        true,
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Out", ConnectionType_Float},
                        },
                },

                {
                        "Divide",
                        true,
                        // Input connections
                        {
                                {"Input1", ConnectionType_Float},
                                {"Input2", ConnectionType_Float},
                        },
                        // Output
                        {
                                {"Output1", ConnectionType_Float},
                                {"Output2", ConnectionType_Float},
                                {"Output3", ConnectionType_Float},
                        },
                },

        };
*/

static SampleExtension se(nullptr);
/*
static struct NodeType nodeTypes[] =
        {
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                {
                        "Texture",
                        true,
                        &se,
                        // Input connections
                        {
                                {"Write", "Texture"},
                        },
                        // Output
                        {
                                {"Read", "Texture"},
                        },
                },
                {
                        "Combine All",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"diffuseSpecularLighted", "Texture"},
                                {"depthMap", "Texture"},
                        },
                        // Output
                        {
                                {"finalColor", "Texture"},

                        },
                        //combine inputs
                        true,
                },
                {
                        "Combine All SSAO",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"diffuseSpecularLighted", "Texture"},
                                {"ambient", "Texture"},
                                {"ssao", "Texture"},
                                {"depthMap", "Texture"},
                        },
                        // Output
                        {
                                {"finalColor", "Texture"},
                        },
                        //combine inputs
                        true,
                },
                {
                        "depthPrePass",
                        false,
                        nullptr,
                        // Input connections
                        {
                        },
                        // Output
                        {
                                {"depthMap", "Texture"},
                        },
                },
                {
                        "GUI",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"GUISampler", "Texture"},
                        },
                        // Output
                        {
                                {"GUI", "Texture"},
                        },
                },
                {
                        "Models",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"shadowSamplerDirectional", "Texture"},
                                {"shadowSamplerPoint", "Texture"},
                                {"ambientSampler", "Texture"},
                                {"diffuseSampler", "Texture"},
                                {"specularSampler", "Texture"},
                                {"opacitySampler", "Texture"},
                                {"normalSampler", "Texture"},
                        },
                        // Output
                        {
                                {"diffuseAndSpecularLightedColor", "Texture"},
                                {"ambientColor", "Texture"},
                                {"normalOutput", "Texture"},

                        },
                },

                {
                        "ModelsTransparent",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"shadowSamplerDirectional", "Texture"},
                                {"shadowSamplerPoint", "Texture"},
                                {"ambientSampler", "Texture"},
                                {"diffuseSampler", "Texture"},
                                {"specularSampler", "Texture"},
                                {"opacitySampler", "Texture"},
                                {"normalSampler", "Texture"},
                        },
                        // Output
                        {
                                {"diffuseAndSpecularLightedColor", "Texture"},
                                {"ambientColor", "Texture"},
                                {"normalOutput", "Texture"},

                        },
                },
                {
                        "ShadowMapDirectional",
                        false,
                        nullptr,
                        // Input connections
                        {
                        },
                        // Output
                        {

                                {"directionalShadowMap", "Texture"},

                        },
                },
                {
                        "ShadowMapPoint",
                        false,
                        nullptr,
                        // Input connections
                        {
                        },
                        // Output
                        {
                                {"PointShadowMap", "Texture"},
                        },
                },
                {
                        "SkyCube",
                        false,
                        nullptr,
                        // Input connections
                        {
                        },
                        // Output
                        {
                                {"SkyBox", "Texture"},
                                {"NormalOutput", "Texture"},
                        },
                },
                {
                        "SSAO",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"depthMapSampler", "Texture"},
                                {"normalMapSampler", "Texture"},
                                {"ssaoNoiseSampler", "Texture"},
                        },
                        // Output
                        {
                                {"occlusion", "Texture"},
                        },
                },
                {
                        "SSAO",
                        false,
                        nullptr,
                        // Input connections
                        {
                                {"ssaoResultSampler", "Texture"},
                        },
                        // Output
                        {
                                {"occlusion", "Texture"},
                        },
                },

                {
                        "Multiply",
                        true,
                        nullptr,
                        // Input connections
                        {
                                {"Input1", "Float"},
                                {"Input2", "Float"},
                        },
                        // Output
                        {
                                {"Out", "Float"},
                        },
                },

                {
                        "Add",
                        true,
                        nullptr,
                        // Input connections
                        {
                                {"Input1", "Integer"},
                                {"Input2", "Integer"},
                        },
                        // Output
                        {
                                {"Out", "Integer"},
                        },
                },

                {
                        "Divide",
                        true,
                        nullptr,
                        // Input connections
                        {
                                {"Input1", "Float"},
                                {"Input2", "Float"},
                        },
                        // Output
                        {
                                {"Output1", "Float"},
                                {"Output2", "Float"},
                                {"Output3", "Float"},
                        },
                },
        };
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ShowExampleAppCustomNodeGraph(NodeGraph& nodeGraph, bool *opened) {
    ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Example: Custom Node Graph", opened)) {
        ImGui::End();
        return;
    }

    ImGui::SameLine();
    nodeGraph.display();
    ImGui::End();
}

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)

#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#endif

// Include glfw3.h after our OpenGL definitions

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **) {
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
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
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
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
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

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::vector <NodeType* > nodeTypeVector;

    NodeType* textureNodeType = new NodeType();
    textureNodeType->combineInputs = true;
    textureNodeType->editable = true;
    //textureNodeType->nodeExtension = nullptr;
    textureNodeType->name = "Texture";
    textureNodeType->inputConnections.emplace_back(ConnectionDesc{"Write", "Texture"});
    textureNodeType->outputConnections.emplace_back(ConnectionDesc{"Read", "Texture"});
    nodeTypeVector.emplace_back(textureNodeType);

    NodeType* combineAllNodeType = new NodeType();
    combineAllNodeType->combineInputs = true;
    combineAllNodeType->editable = false;
    //combineAllNodeType->nodeExtension = nullptr;
    combineAllNodeType->name = "Combine All";
    combineAllNodeType->inputConnections.emplace_back(ConnectionDesc{"diffuseSpecularLighted", "Texture"});
    combineAllNodeType->inputConnections.emplace_back(ConnectionDesc{"depthMap", "Texture"});
    combineAllNodeType->outputConnections.emplace_back(ConnectionDesc{"finalColor", "Texture"});
    nodeTypeVector.emplace_back(combineAllNodeType);

    NodeType* combineAllSSAONodeType = new NodeType();
    combineAllSSAONodeType->combineInputs = true;
    combineAllSSAONodeType->editable = false;
    //combineAllSSAONodeType->nodeExtension = nullptr;
    combineAllSSAONodeType->name = "Combine All SSAO";
    combineAllSSAONodeType->inputConnections.emplace_back(ConnectionDesc{"diffuseSpecularLighted", "Texture"});
    combineAllSSAONodeType->inputConnections.emplace_back(ConnectionDesc{"ambient", "Texture"});
    combineAllSSAONodeType->inputConnections.emplace_back(ConnectionDesc{"ssao", "Texture"});
    combineAllSSAONodeType->inputConnections.emplace_back(ConnectionDesc{"depthMap", "Texture"});
    combineAllSSAONodeType->outputConnections.emplace_back(ConnectionDesc{"finalColor", "Texture"});
    nodeTypeVector.emplace_back(combineAllSSAONodeType);

    NodeType* guiNodeType = new NodeType();
    guiNodeType->combineInputs = false;
    guiNodeType->editable = false;
    guiNodeType->nodeExtensionConstructor = [](const NodeType* nodeType) -> NodeExtension* { return new SampleExtension(nodeType);};
    guiNodeType->name = "GUI";
    guiNodeType->inputConnections.emplace_back(ConnectionDesc{"GUISampler", "Texture"});
    guiNodeType->outputConnections.emplace_back(ConnectionDesc{"GUI", "Texture"});
    nodeTypeVector.emplace_back(guiNodeType);

    NodeType* depthPrePassNodeType = new NodeType();
    depthPrePassNodeType->combineInputs = false;
    depthPrePassNodeType->editable = false;
    depthPrePassNodeType->nodeExtensionConstructor = [](const NodeType* nodeType) -> NodeExtension* { return new SampleExtension(nodeType);};
    depthPrePassNodeType->name = "depthPrePass";
    depthPrePassNodeType->outputConnections.emplace_back(ConnectionDesc{"depthMap", "Texture"});
    nodeTypeVector.emplace_back(depthPrePassNodeType);


    SampleEditorExtension sampleEditorExtension;
    //NodeGraph* nodeGraph = new NodeGraph(nodeTypeVector, false, &sampleEditorExtension);

    std::unordered_map<std::string, std::function<EditorExtension*()>> possibleEditorExtensions;
    possibleEditorExtensions[sampleEditorExtension.getName()] = []() -> EditorExtension* {return new SampleEditorExtension();};
    std::unordered_map<std::string, std::function<NodeExtension*(const NodeType*)>> possibleNodeExtensions;
    possibleNodeExtensions[se.getName()] = [](const NodeType* nodeType) -> NodeExtension* {return new SampleExtension(nodeType);};

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();
    NodeGraph* nodeGraph = NodeGraph::deserialize("Nodes.xml", possibleEditorExtensions, possibleNodeExtensions);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
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

        bool show_test_window = true;
        ShowExampleAppCustomNodeGraph(*nodeGraph, &show_test_window);

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