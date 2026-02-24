cmake_minimum_required(VERSION 3.21)

# Project name and language
project(CurveNetModeller VERSION 0.1.0 LANGUAGES CXX)

# Use C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    # src/model/CurveModel.cpp
    # src/view/MainWindow.cpp
)

# Create executable
add_executable(CurveNetModeller ${SOURCES})

# Include root directory
target_include_directories(CurveNetModeller PRIVATE src)

# Link libraries
target_link_libraries(CurveNetModeller PRIVATE
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    OpenGL::GL
)