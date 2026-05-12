#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <numbers>
#include <fstream>

// Fontos: A glad.h-t MINDIG a glfw3.h előtt kell include-olni!
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ImGui headerek
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLM headerek
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GlEngine
#include "gl_engine/shared_shaders.hpp"

// Model
#include "model/element/bezier_node.h"
#include "model/scene.hpp"

// Views
#include "editor/workspace/viewport.hpp"

// Controller
#include "editor/controller/bezier_curve_controller.hpp"

// Util
#include "editor/controller/curve_mesh_controller.h"
#include "editor/interface/curve_mesh_interface.hpp"
#include "editor/interface/global_actions_interface.hpp"
#include "model/element/coons_surface.h"
#include "util/screenshot.hpp"
#include "editor/view/coons_surface_view.h"

#include "editor/view/floor_grid.h"
#include "editor/view/skybox.h"
#include "editor/workspace/model_to_interface.hpp"


static int Width = 1280, Height = 720;
// Ablak átméretezésekor lefutó függvény (OpenGL viewport frissítése)
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Width = width;
    Height = height;

    glViewport(0, 0, width, height);
}

int GLFW_INIT(GLFWwindow** window) {
    // ------------------------------------------------------------------
    // 1. GLFW inicializálása és ablak létrehozása
    // ------------------------------------------------------------------
    if (!glfwInit()) {
        std::cerr << "Hiba: GLFW inicializalasa sikertelen!" << std::endl;
        return -1;
    }

    // OpenGL 3.3 Core profile beállítása
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    // Ablak létrehozása
    *window = glfwCreateWindow(Width, Height, "CurveNetModeller", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Hiba: GLFW ablak letrehozasa sikertelen!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
    glfwSwapInterval(1); // V-Sync bekapcsolása

    GLFWimage ico[1];
    ico[0].pixels = stbi_load("resources/icons/icon.png",  &ico[0].width, &ico[0].height, 0, 4);
    glfwSetWindowIcon(*window, 1, ico);
    stbi_image_free(ico[0].pixels);

    return 0;
}

int OPENGL_INIT(GLFWwindow* window) {
    // ------------------------------------------------------------------
    // 2. GLAD inicializálása (OpenGL függvények betöltése)
    // ------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Hiba: GLAD inicializalasa sikertelen!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    return 0;
}

int IMGUI_INIT(GLFWwindow* window) {
    // ------------------------------------------------------------------
    // 3. ImGui inicializálása
    // ------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Tobb ablakos varazslat
    //io.IniFilename = nullptr;
    ImGui::StyleColorsDark(); // Sötét téma beállítása

    // Backendek inicializálása
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return 0;
}

void Save(const CurveMesh& scene) {
    std::ofstream fs("output/save.json");

    if (!fs.is_open()) {
        std::cout << "[-] Failed to open output file" << std::endl;
        return;
    }

    {
#ifdef NDEBUG
        cereal::JSONOutputArchive archive(fs, cereal::JSONOutputArchive::Options::NoIndent());
#else
        cereal::JSONOutputArchive archive(fs);
#endif
        archive(cereal::make_nvp("SaveData", scene));
    }

    fs.close();
    std::cout << "[+] Successfully saved to output/save.json" << std::endl;
}

// CoonsSurface LoadSurface() {
//     std::ifstream fs("output/save.json");
//
//     std::cout << ReadAllText("output/save.json") << std::endl;
//
//     CoonsSurface loaded;
//     cereal::JSONInputArchive archive(fs);
//     archive(cereal::make_nvp("SaveData", loaded)); // A varázslat itt történik!
//
//     return loaded;
// }

int main() {
    GLFWwindow* mainWindow;

    if (GLFW_INIT(&mainWindow) || mainWindow == nullptr) {
        std::cout << "[-] Failed to initialize GLFW" << std::endl;
        return -1;
    }
    if (OPENGL_INIT(mainWindow)) {
        std::cout << "[-] Failed to initialize OpenGL/GLAD" << std::endl;
        return -2;
    }
    if (IMGUI_INIT(mainWindow)) {
        std::cout << "[-] Failed to initialize IMGUI" << std::endl;
        return -3;
    }

    SharedShaders::Register("solid_color", std::make_shared<Shader>("resources/shaders/trafo.vert", "resources/shaders/color.frag"));
    SharedShaders::Register("shaded", std::make_shared<Shader>("resources/shaders/trafo_norm.vert", "resources/shaders/shaded.frag"));
    SharedShaders::Register("skybox", std::make_shared<Shader>("resources/shaders/skybox.vert", "resources/shaders/skybox.frag"));

    //ModelToInterface::Register(std::type_index(typeid(Point)), )


    PerspectiveCamera cam(
        glm::vec3(0, 0, 5),
        glm::vec3(std::numbers::pi_v<float>, std::numbers::pi_v<float>, 0),
        1,
        0.1f,
        100.0f
    );
    cam.Init();


    std::shared_ptr<Viewport> viewport = std::make_shared<Viewport>();
    Workspaces::viewport = viewport;

    std::shared_ptr<Properties> properties = std::make_shared<Properties>();
    Workspaces::properties = properties;



    std::shared_ptr<CurveMeshController> curveMeshController = std::make_shared<CurveMeshController>();
    curveMeshController->AddNewCurve();
    curveMeshController->GetModel()->GetEdges()[0]->AddNode(std::make_shared<BezierNode>(glm::vec3(2, -1, 0)));
    curveMeshController->AddNewSurface();

    Workspaces::properties.lock()->AddInterface(std::make_shared<CurveMeshInterface>(curveMeshController));
    Workspaces::properties.lock()->AddInterface(std::make_shared<GlobalActionsInterface>());


    FloorGrid floor;
    SkyBox skybox("resources/images/skybox/sunset.png", AlphaMode::AlphaBlend, 0.25f);

    // MAIN LOOP
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();

        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // SCENE
        viewport->BindFrameBuffer();
            glEnable(GL_DEPTH_TEST);
            viewport->viewportBuffer->Clear();

                Camera::activeCamera->UpdateFrameSize(viewport->viewportBuffer->Width, viewport->viewportBuffer->Height);
                curveMeshController->Present();
                skybox.Draw();
                floor.Draw();

            glDisable(GL_DEPTH_TEST);
        viewport->UnbindFrameBuffer();
        glViewport(0, 0, Width, Height);

        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();

            //UI(*viewport);
            Workspaces::viewport.lock()->Draw();
            Workspaces::properties.lock()->Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mainWindow);
    }

    Save(*curveMeshController->GetModel());

    // ------------------------------------------------------------------
    // 6. Takarítás (Erőforrások felszabadítása)
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}

// float* proj = cam.GetActiveProjectionMatrix();
// std::cout << proj[0] << " " << proj[1] << " " << proj[2] << " " << proj[3] << std::endl;
// std::cout << proj[4] << " " << proj[5] << " " << proj[6] << " " << proj[7] << std::endl;
// std::cout << proj[8] << " " << proj[9] << " " << proj[10] << " " << proj[11] << std::endl;
// std::cout << proj[12] << " " << proj[13] << " " << proj[14] << " " << proj[15] << std::endl;

// float* view = cam.GetActiveViewMatrix();
// std::cout << view[0] << " " << view[1] << " " << view[2] << " " << view[3] << std::endl;
// std::cout << view[4] << " " << view[5] << " " << view[6] << " " << view[7] << std::endl;
// std::cout << view[8] << " " << view[9] << " " << view[10] << " " << view[11] << std::endl;
// std::cout << view[12] << " " << view[13] << " " << view[14] << " " << view[15] << std::endl;


// Ha kéne a tobb ablakos varazslat
// if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
//     GLFWwindow* backup_current_context = glfwGetCurrentContext();
//     ImGui::UpdatePlatformWindows();
//     ImGui::RenderPlatformWindowsDefault();
//     glfwMakeContextCurrent(backup_current_context);
// }


// BezierCurveController c1;
// c1.AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-3, -1, -2), glm::vec3(-1,  1, -2)));
// c1.AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 1,  1, -2), glm::vec3( 3, -1, -2)));
// BezierCurveController c2;
// c2.AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-3, -1,  2), glm::vec3(-1,  1,  2)));
// c2.AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 1,  1,  2), glm::vec3( 3, -1,  2)));
// BezierCurveController d1;
// d1.AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-2,  1, -3), glm::vec3(-2, -1, -1)));
// d1.AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-2, -1,  1), glm::vec3(-2,  1,  3)));
// BezierCurveController d2;
// d2.AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 2,  1, -3), glm::vec3( 2, -1, -1)));
// d2.AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 2, -1,  1), glm::vec3( 2,  1,  3)));

// CoonsSurface coons(c1.GetModel(), c2.GetModel(), d1.GetModel(), d2.GetModel());
// CoonsSurfaceView coonsView;
// coonsView.Update(coons);

// coons.CoonsSurfaceChanged += [&](){ coonsView.Update(coons); };

// c = std::make_unique<BezierCurveController>();
// c->AddNode(std::make_shared<BezierNode>(glm::vec3(-5.0f,  5.0f, 0.0f), glm::vec3(-7.0f,  3.0f, 0.0f), glm::vec3(-3.0f,  7.0f, 0.0f)));
// c->AddNode(std::make_shared<BezierNode>(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(-2.0f, -2.0f, 0.0f), glm::vec3( 2.0f,  2.0f, 0.0f)));
// c->AddNode(std::make_shared<BezierNode>(glm::vec3( 5.0f, -5.0f, 0.0f), glm::vec3( 3.0f, -7.0f, 0.0f), glm::vec3( 7.0f, -3.0f, 0.0f)));
// c->SyncViews();

// c->Present();
// coonsView.Draw();
// c1.Present();
// c2.Present();
// d1.Present();
// d2.Present();