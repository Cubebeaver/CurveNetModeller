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

// Ablak átméretezésekor lefutó függvény (OpenGL viewport frissítése)
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CurveNetModeller - Teszt", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Hiba: GLFW ablak letrehozasa sikertelen!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // V-Sync bekapcsolása

    // ------------------------------------------------------------------
    // 2. GLAD inicializálása (OpenGL függvények betöltése)
    // ------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Hiba: GLAD inicializalasa sikertelen!" << std::endl;
        return -1;
    }

    // ------------------------------------------------------------------
    // 3. ImGui inicializálása
    // ------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark(); // Sötét téma beállítása

    // Backendek inicializálása
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // ------------------------------------------------------------------
    // 4. GLM Teszt (Egyszerű matematika)
    // ------------------------------------------------------------------
    // Létrehozunk egy vektort, amit majd kiíratunk az ImGui felületre, 
    // bizonyítva, hogy a GLM is működik.
    glm::vec3 testVector(1.0f, 2.5f, -3.0f);

    // ------------------------------------------------------------------
    // 5. Fő renderelő ciklus
    // ------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // Események feldolgozása (billentyűzet, egér)
        glfwPollEvents();

        // ImGui új képkocka indítása
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- ImGui UI definíció kezdete ---
        ImGui::Begin("Konyvtar Tesztelo Ablak");
        ImGui::Text("Hello CurveNetModeller!");
        ImGui::Separator();
        ImGui::Text("A konyvtarak allapota:");
        ImGui::BulletText("GLFW: MUKODIK (Ablak megnyilt)");
        ImGui::BulletText("GLAD: MUKODIK (OpenGL kontextus el: %s)", glGetString(GL_VERSION));
        ImGui::BulletText("ImGui: MUKODIK (Ezt a szoveget latod)");
        
        // GLM érték kiíratása
        ImGui::BulletText("GLM: MUKODIK (Teszt vektor: X=%.1f, Y=%.1f, Z=%.1f)", 
                          testVector.x, testVector.y, testVector.z);
        
        // Egy kis interakció
        if (ImGui::Button("Nyomj meg!")) {
            testVector.x += 1.0f; // Gombnyomásra módosítjuk a vektort
        }
        ImGui::End();
        // --- ImGui UI definíció vége ---

        // ImGui renderelés előkészítése
        ImGui::Render();

        // OpenGL háttér törlése egy szép sötétkék/szürke színnel
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui rárajzolása a képernyőre
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Bufferek cseréje (képkocka megjelenítése)
        glfwSwapBuffers(window);
    }

    // ------------------------------------------------------------------
    // 6. Takarítás (Erőforrások felszabadítása)
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}