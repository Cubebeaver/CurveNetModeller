#pragma once

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shared_shaders.hpp"

class FloorGrid {
private:
    std::unique_ptr<Mesh> grid;
    std::unique_ptr<Mesh> axis;
    std::unique_ptr<Material> mat;

public:
    FloorGrid() {
        SetupGrid();
        SetupAxis();

        mat = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    }

    void Draw() {
        mat->Bind();
        mat->SetMat4("Model", glm::mat4(1.0f));
        mat->SetMat4("View", Camera::activeCamera->matView);
        mat->SetMat4("Projection", Camera::activeCamera->matProjection);

        glLineWidth(1);
        mat->SetVec4("color", glm::vec4(0.2f, 0.2f, 0.2f, 0.5f));
        mat->Bind();
        grid->Draw(GL_LINES);

        glLineWidth(2);
        mat->SetVec4("color", glm::vec4(1, 0, 0, 1));
        mat->Bind();
        axis->DrawPartial(0, 2, GL_LINES);
        mat->SetVec4("color", glm::vec4(0, 1, 0, 1));
        mat->Bind();
        axis->DrawPartial(2, 4, GL_LINES);
        mat->SetVec4("color", glm::vec4(0, 0, 1, 1));
        mat->Bind();
        axis->DrawPartial(4, 6, GL_LINES);
    }

private:
    void SetupGrid() {
        std::vector<float> verts;
        verts.reserve(21 * 2 * 2 * 3);
        std::vector<GLuint> idxs;
        idxs.reserve(21 * 2 * 2);

        for (int i = 0; i < 21; i++) {
            verts.push_back(-10.0f + i);
            verts.push_back(0);
            verts.push_back(-10.0f);

            verts.push_back(-10.0f + i);
            verts.push_back(0);
            verts.push_back(10.0f);
        }

        for (int i = 0; i < 21; i++) {
            verts.push_back(-10.0f);
            verts.push_back(0);
            verts.push_back(-10.0f + i);

            verts.push_back(10.0f);
            verts.push_back(0);
            verts.push_back(-10.0f + i);
        }

        for (int i = 0; i < 21 * 2 * 2; i++) {
            idxs.push_back(i);
        }

        grid = std::make_unique<Mesh>(verts, idxs);
        grid->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();
    }

    void SetupAxis() {
        std::vector<float> verts;
        verts.reserve(4 * 3);
        std::vector<GLuint> idxs;
        idxs.reserve(6);

        verts.push_back(0);verts.push_back(0);verts.push_back(0);
        verts.push_back(2);verts.push_back(0);verts.push_back(0);
        verts.push_back(0);verts.push_back(2);verts.push_back(0);
        verts.push_back(0);verts.push_back(0);verts.push_back(2);

        idxs.push_back(0);idxs.push_back(1);
        idxs.push_back(0);idxs.push_back(2);
        idxs.push_back(0);idxs.push_back(3);

        axis = std::make_unique<Mesh>(verts, idxs);
        axis->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();
    }
};
