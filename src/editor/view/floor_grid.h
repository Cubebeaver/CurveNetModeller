#pragma once

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class FloorGrid {
private:
    std::unique_ptr<gl_engine::Mesh> grid;
    std::unique_ptr<gl_engine::Mesh> axis;
    std::unique_ptr<gl_engine::Material> mat;

public:
    FloorGrid();

    void Draw();

private:
    void SetupGrid();
    void SetupAxis();
};
