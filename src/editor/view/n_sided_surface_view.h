#pragma once

#include <glm/glm.hpp>
#include "model/element/n_sided_surface.hpp"

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class NSidedSurfaceView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> material;

public:
    NSidedSurfaceView();

    void Update(const NSidedSurface& surfaceModel, int resolution = 50);

    void Draw();

private:
    std::vector<glm::vec3> GenRenderPoints(const NSidedSurface& surface, int resolution = 10);
    std::vector<glm::vec3> GenRenderNormals(const NSidedSurface& surface, int resolution = 10);
};
