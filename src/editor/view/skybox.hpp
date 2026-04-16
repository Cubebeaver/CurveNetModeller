#pragma once

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"
using namespace gl_engine;

class SkyBox {
private:
    std::unique_ptr<Mesh> quad;
    std::unique_ptr<Material> mat;
    std::shared_ptr<Texture> tex;

public:
    SkyBox(const std::string& imagePath, AlphaMode alphaMode = AlphaMode::Opaque, float brightness = 0.5f) {
        SetupGrid();

        mat = std::make_unique<Material>(SharedShaders::Get("skybox"));
        tex = std::make_shared<Texture>(imagePath, alphaMode, true, GL_CLAMP_TO_EDGE);

        mat->AddTexture("skyboxTexture", tex);
        mat->SetFloat("brightness", brightness);
    }

    void Draw() {
        glDepthFunc(GL_LEQUAL);
        quad->Bind();

        mat->Bind();
        mat->SetMat4("invView", Camera::activeCamera->matInvView);
        mat->SetMat4("invProjection", Camera::activeCamera->matInvProjection);

        quad->Draw();
        glDepthFunc(GL_LESS);
    }

private:
    void SetupGrid() {
        std::vector<float> verts;
        verts.reserve(4 * 3);
        std::vector<GLuint> idxs;
        idxs.reserve(6);

        verts.push_back(-1); verts.push_back(-1); verts.push_back(-0.9999f);
        verts.push_back(-1); verts.push_back( 1); verts.push_back(-0.9999f);
        verts.push_back( 1); verts.push_back(-1); verts.push_back(-0.9999f);
        verts.push_back( 1); verts.push_back( 1); verts.push_back(-0.9999f);

        idxs.push_back(0);idxs.push_back(3);idxs.push_back(2);
        idxs.push_back(0);idxs.push_back(1);idxs.push_back(3);

        quad = std::make_unique<Mesh>(verts, idxs);
        quad->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();
    }
};
