#pragma once

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class SkyBox {
private:
    std::unique_ptr<gl_engine::Mesh> quad;
    std::unique_ptr<gl_engine::Material> mat;
    std::shared_ptr<gl_engine::Texture> tex;

public:
    SkyBox(const std::string& imagePath, gl_engine::AlphaMode alphaMode = gl_engine::AlphaMode::Opaque, float brightness = 0.5f);

    void Draw();

private:
    void SetupGrid();
};
