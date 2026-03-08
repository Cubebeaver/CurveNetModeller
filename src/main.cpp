#include <iostream>
#include <numbers>

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
#include "gl_engine/shader.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/texture.hpp"
#include "gl_engine/object.hpp"
#include "gl_engine/framebuffer.hpp"
#include "gl_engine/camera.hpp"

// Model
#include "model/bezier_node.h"
#include "model/bezier_curve.h"

// Views
#include "view/bezier_node_view.hpp"
#include "view/bezier_curve_view.hpp"
#include "workspace/viewport.hpp"

// Controller
#include "controller/bezier_curve_controller.hpp"

// Util
#include "util/screenshot.hpp"


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
    glEnable(GL_CULL_FACE);

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    return 0;
}

int IMGUI_INNIT(GLFWwindow* window) {
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


bool checked = false;
BezierCurveController* c;
void UI(const Viewport& viewport) {
    // ImGui új képkocka indítása
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    // --- ImGui UI definíció kezdete ---
    
    
    viewport.Draw();
    
    
    ImGui::SetNextWindowSize(ImVec2(560, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin("Properties");
    
        ImGui::SeparatorText("Curve");
        if (ImGui::Button("Add control pont")) {
            c->AddNode();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove control point")) {
            c->RemoveNode();
        }

        ImGui::SeparatorText("Node");
        ImGui::Text("Set mode: ");
        ImGui::SameLine();
        if (ImGui::Button("Symmetric")) {
            c->SetNodeMode(HandleMode::Symmetric);
        }
        ImGui::SameLine();
        if (ImGui::Button("Aligned")) {
            c->SetNodeMode(HandleMode::Aligned);
        }
        ImGui::SameLine();
        if (ImGui::Button("Free")) {
            c->SetNodeMode(HandleMode::Free);
        }

        ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
        ImGui::SeparatorText("Tests");
        if (ImGui::Button("Hello World!")) {
            std::cout << "Hello World!" << std::endl;
        }
        ImGui::SameLine();
        if (ImGui::Button("Save viewport screenshot")) {
            CaptureScreenshot(viewport.viewportBuffer->fbo, viewport.viewportBuffer->Width, viewport.viewportBuffer->Height);
        }
        ImGui::Checkbox("Switch color", &checked);

    ImGui::End();
    // --- ImGui UI definíció vége ---
    
    // ImGui renderelés előkészítése
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Ha kéne a tobb ablakos varazslat
    // if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    //     glfwMakeContextCurrent(backup_current_context);
    // }
}

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
    if (IMGUI_INNIT(mainWindow)) {
        std::cout << "[-] Failed to initialize IMGUI" << std::endl;
        return -3;
    }

    const std::vector<float> vertexBuffer {
//       posx   posy   posz     norx  nory  norz     r  g  b  a     u  v
        -1.0f, -1.0f,  0.0f,    0.0f, 0.0f, 1.0f,    1, 0, 0, 1,    0, 0,
        -1.0f,  1.0f,  0.0f,    0.0f, 0.0f, 1.0f,    0, 1, 0, 1,    0, 1,
         1.0f, -1.0f,  0.0f,    0.0f, 0.0f, 1.0f,    0, 0, 1, 1,    1, 0,
         1.0f,  1.0f,  0.0f,    0.0f, 0.0f, 1.0f,    1, 1, 0, 1,    1, 1
    };
    const std::vector<GLuint> indexBuffer {
        0, 2, 1,
        2, 3, 1
    };

    
    Mesh mesh(vertexBuffer, indexBuffer);
    mesh.AddAttribPointer(3, GL_FLOAT, false)
        .AddAttribPointer(3, GL_FLOAT, false)
        .AddAttribPointer(4, GL_FLOAT, false)
        .AddAttribPointer(2, GL_FLOAT, false)
        .FinishVertexAttribs();

    Shader shader("resources/shaders/screen.vert", "resources/shaders/screen.frag");

    Texture texture("resources/images/Blueprint.png", AlphaMode::AplhaClip);

    Material orange(&shader);
    orange.SetVec4("color", glm::vec4(1, 0.5f, 0, 1));
    orange.SetVec4("tint", glm::vec4(1, 0, 0, 1));
    
    Material blue(&shader);
    blue.AddTexture("albedo", &texture);
    blue.SetVec4("color", glm::vec4(0, 0, 1, 1));
    blue.SetVec4("tint", glm::vec4(0, 0, 1, 1));

    Transform trafo;

    Object obj1(&trafo, &mesh, &orange);
    Object obj2(&trafo, &mesh, &blue);


    Viewport vp;
    c = new BezierCurveController(&vp);


    OrthoCamera cam(glm::vec3(0, 0, 1), glm::vec3(std::numbers::pi_v<float>, std::numbers::pi_v<float>, 0), 1, 0.1f, 100.0f);
    cam.Init();

    // MAIN LOOP
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();
        
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Scene
        vp.BindFrameBuffer();

        Camera::activeCamera->UpdateFrameSize(vp.viewportBuffer->Width, vp.viewportBuffer->Height);

        if (checked) obj1.Draw();
        else         obj2.Draw();
        c->Present();
        vp.UnbindFrameBuffer();
        glViewport(0, 0, Width, Height);

        UI(vp);

        glfwSwapBuffers(mainWindow);
    }

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