#pragma once

#include <glm/glm.hpp>
#include <model/element/coons_surface.h>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class CoonsSurfaceView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> material;

public:
    CoonsSurfaceView();

    void Update(const CoonsSurface& surfaceModel, int resolution = 20);

    void Draw();
};
