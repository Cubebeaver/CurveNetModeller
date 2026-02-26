#include <iostream>

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
#include "gl_engine/mesh.hpp"
#include "gl_engine/texture.hpp"
#include "gl_engine/framebuffer.hpp"


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
    *window = glfwCreateWindow(Width, Height, "CurveNetModeller - Teszt", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Hiba: GLFW ablak letrehozasa sikertelen!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
    glfwSwapInterval(1); // V-Sync bekapcsolása

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

void UI(FrameBuffer& viewport) {
    // ImGui új képkocka indítása
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    // --- ImGui UI definíció kezdete ---
    ImGui::Begin("Konyvtar Tesztelo Ablak");
    ImGui::Text("Hello CurveNetModeller!");
    ImGui::Separator();
    ImGui::Text("A konyvtarak allapota:");
    ImGui::BulletText("GLFW: MUKODIK (Ablak megnyilt)");
    ImGui::BulletText("GLAD: MUKODIK (OpenGL kontextus el: %s)", glGetString(GL_VERSION));
    ImGui::BulletText("ImGui: MUKODIK (Ezt a szoveget latod)");
    
    // Egy kis interakció
    if (ImGui::Button("Nyomj meg!")) {
        std::cout << "Hello World" << std::endl;
    }
    ImGui::End();
    // --- ImGui UI definíció vége ---

    ImGui::Begin("Viewport");
    
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    viewport.UpdateSize(viewportPanelSize.x, viewportPanelSize.y);
    ImGui::Image((ImTextureID)viewport.colorBuffer, ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2(0, 1), ImVec2(1, 0));
    
    ImGui::End();

    
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
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    if (OPENGL_INIT(mainWindow)) {
        std::cout << "Failed to initialize OpenGL/GLAD" << std::endl;
        return -2;
    }
    if (IMGUI_INNIT(mainWindow)) {
        std::cout << "Failed to initialize IMGUI" << std::endl;
        return -3;
    }

    

    const std::vector<float> vertexBuffer {
//       posx   posy   posz     norx  nory  norz     r  g  b  a     u  v
        -0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    1, 0, 0, 1,    0, 0,
        -0.5f,  0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    0, 1, 0, 1,    0, 1,
         0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    0, 0, 1, 1,    1, 0,
         0.5f,  0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    1, 1, 0, 1,    1, 1
    };
    const std::vector<GLuint> indexBuffer {
        0, 2, 1,
        2, 3, 1
    };

    Mesh<float> mesh(vertexBuffer, indexBuffer);
    mesh.AddAttribPointer(3, GL_FLOAT, false)
        .AddAttribPointer(3, GL_FLOAT, false)
        .AddAttribPointer(4, GL_FLOAT, false)
        .AddAttribPointer(2, GL_FLOAT, false)
        .FinishVertexAttribs();

    Shader shader("resources/shaders/screen.vert", "resources/shaders/screen.frag");
    shader.Use();
    glUniform1i(glGetUniformLocation(shader.program, "albedo"), 0);
    glUniform4f(glGetUniformLocation(shader.program, "color"), 0, 0, 0, 0);

    Texture texture("resources/images/Blueprint.png", AlphaMode::AplhaClip);
    texture.BindToUnit(0);

    FrameBuffer framebuffer(256, 256);





    // MAIN LOOP
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();
        
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Scene
        framebuffer.Bind();
        shader.Use();
        texture.BindToUnit(0);
        mesh.Draw();
        framebuffer.Unbind();
        glViewport(0, 0, Width, Height);

        UI(framebuffer);

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