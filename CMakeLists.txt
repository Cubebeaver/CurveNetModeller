cmake_minimum_required(VERSION 3.21)

# ==========================================
# 1. ALAPVETŐ BEÁLLÍTÁSOK
# ==========================================
# Projekt neve és nyelve
project(CurveNetModeller VERSION 0.1.0 LANGUAGES CXX)

# Modern C++20 szabvány kikényszerítése
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Generál egy compile_commands.json fájlt, amit az IDE-k (pl. VS Code) imádnak az intellisense-hez
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 


# ==========================================
# 2. FÜGGŐSÉGEK (DEPENDENCIES) KERESÉSE
# ==========================================
# vcpkg által biztosított csomagok
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Header-only csomagok útvonalának megkeresése
find_path(STB_INCLUDE_DIR "stb_image.h" REQUIRED)


# ==========================================
# 3. FORRÁSFÁJLOK (SOURCES)
# ==========================================
# Közös kód: Ezt a főprogram és a teszt is használni fogja (az üzleti logika)
set(MODEL_SOURCES
        src/model/element/bezier_curve.cpp
        src/model/element/bezier_node.cpp
        src/model/element/coons_surface.cpp
        src/model/element/point.cpp
        src/model/object/object.cpp
        src/model/object/curve_mesh.cpp
        src/model/object/transform.cpp
        src/model/constraint/point_on_curve_constraint.cpp
        src/model/constraint/same_point_position_constraint.cpp
)

# Csak a főprogramhoz tartozó fájlok (Ablakkezelés, UI)
set(APP_SOURCES
    src/main.cpp
        src/editor/controller/curve_mesh_controller.cpp
        src/editor/interface/element_interfaces.cpp
        src/editor/view/bezier_node_view.cpp
        src/editor/view/coons_surface_view.cpp
        src/editor/view/curve_curvature_comb_view.cpp
        src/editor/view/curve_view.cpp
        src/editor/view/floor_grid.cpp
        src/editor/view/render_point_generator.cpp
        src/editor/view/skybox.cpp
        src/util/stb_implementation.cpp
)

# Az OpenGL motorhoz tartozó fáljok (Shaderek, Objectek)
set(GL_ENGINE_SOURCES
    src/gl_engine/mesh.hpp
    src/gl_engine/shader.hpp
    src/gl_engine/texture.hpp
    src/gl_engine/material.hpp
)

# Csak a tesztekhez tartozó fájlok
set(TEST_SOURCES
        tests/model/test_bezier.cpp
        tests/model/test_transform.cpp
        tests/model/test_coons.cpp
)


# ==========================================
# 4. FŐPROGRAM (CURVENETMODELLER) CÉLPONT
# ==========================================
# Létrehozzuk az .exe-t az APP és a MODEL fájlok összefűzésével
add_executable(CurveNetModeller ${APP_SOURCES} ${GL_ENGINE_SOURCES} ${MODEL_SOURCES})

# Megmondjuk, hol keresse a headereket (#include "...")
target_include_directories(CurveNetModeller PRIVATE 
    src 
    ${STB_INCLUDE_DIR}
)

# Hozzákötjük a szükséges könyvtárakat
target_link_libraries(CurveNetModeller PRIVATE
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    OpenGL::GL
    cereal::cereal
)


# ==========================================
# 5. TESZTELÉS (UNIT TESTS) CÉLPONT
# ==========================================
# Bekapcsoljuk a CMake beépített tesztelő modulját
enable_testing()

# Létrehozzuk a teszt .exe-t a TEST és a MODEL fájlok összefűzésével
add_executable(CurveNetModellerTests ${TEST_SOURCES} ${GL_ENGINE_SOURCES} ${MODEL_SOURCES})

# A teszteknek is látniuk kell a 'src' mappát a modellek behúzásához
target_include_directories(CurveNetModellerTests PRIVATE src)

# Hozzákötjük a Google Test-et és a matematikához a GLM-et
target_link_libraries(CurveNetModellerTests PRIVATE
    GTest::gtest 
    GTest::gtest_main
    glm::glm
    cereal::cereal
)

# Regisztráljuk a teszteket a CTest (a futtató környezet) számára
include(GoogleTest)
gtest_discover_tests(CurveNetModellerTests)


# ==========================================
# 6. ERŐFORRÁSOK (RESOURCES) AUTOMATIKUS MÁSOLÁSA
# ==========================================

# Létrehozunk egy fordítás utáni (POST_BUILD) parancsot a főprogramhoz
add_custom_command(TARGET CurveNetModeller POST_BUILD
    # A CMake beépített, operációs rendszer független másoló parancsát használjuk
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    # HONNAN: A projekt gyökerében lévő 'resources' mappa
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    # HOVÁ: Oda, ahová a CurveNetModeller.exe éppen legenerálódott, egy 'resources' almappába
    "$<TARGET_FILE_DIR:CurveNetModeller>/resources"
)

add_custom_command(TARGET CurveNetModeller POST_BUILD
        # A CMake beépített, operációs rendszer független másoló parancsát használjuk
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        # HONNAN: A projekt gyökerében lévő 'output' mappa
        "${CMAKE_CURRENT_SOURCE_DIR}/output"
        # HOVÁ: Oda, ahová a CurveNetModeller.exe éppen legenerálódott, egy 'output' almappába
        "$<TARGET_FILE_DIR:CurveNetModeller>/output"
)



# --- ADDRESS SANITIZER (ASan) BEÁLLÍTÁSOK MSVC-HEZ ---
if(MSVC)
    # A CMake alapból hozzáadja az /RTC1 flaget Debug módban, ami összeakad az ASan-nal.
    # Ez a két sor kigyomlálja ezt a flaget a default beállításokból.
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCc" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

    # Csak Debug profil esetén kapcsoljuk be
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        message(STATUS "[ASan] AddressSanitizer bekapcsolva a Debug profilhoz!")

        # A célpontod neve (cseréld le a sajátodra, ha nem CurveNetModeller!)
        set(TARGET_NAME CurveNetModeller)

        # Bekapcsoljuk magát az ASan-t
        target_compile_options(${TARGET_NAME} PRIVATE /fsanitize=address)

        # Bekapcsoljuk a debug szimbólumokat, hogy pontos fájlnevet és sorszámot kapjunk hiba esetén
        target_compile_options(${TARGET_NAME} PRIVATE /Zi)

        # A linkernek is tudnia kell az ASan-ról
        target_link_options(${TARGET_NAME} PRIVATE /INCREMENTAL:NO)
    endif()
endif()