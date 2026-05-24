#pragma once

#include <glm/glm.hpp>
#include <model/element/coons_surface.h>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class SurfaceView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> material;

public:
    SurfaceView();

    void Update(const ISurface& surfaceModel, int resolution = 20);

    void Draw();

private:
    std::vector<glm::vec3> GenRenderPoints(const ISurface& surface, int resolution = 20);
    std::vector<glm::vec3> GenRenderNormals(const ISurface& surface, int resolution = 20);
};
