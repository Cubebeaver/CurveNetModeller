#include "bezier_surface_view.h"

#include "gl_engine/camera.hpp"
#include "gl_engine/shared_shaders.hpp"

using namespace gl_engine;

BezierSurfaceView::BezierSurfaceView() {
    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

    lineMat = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    lineMat->SetVec4("color", glm::vec4(1, 1, 0, 1));

    pointMat = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    pointMat->SetVec4("color", glm::vec4(1, 0, 0, 1));

    selectedMat = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    selectedMat->SetVec4("color", glm::vec4(1, 0, 1, 1));
}

void BezierSurfaceView::Update(const BezierSurface& surfaceModel) {
    u = surfaceModel.GetPoints().size();
    if (u > 0)
        v = surfaceModel.GetPoints()[0].size();
    else
        v = 0;

    std::vector<float> verts;

    for (int i = 0; i < u; ++i) {
        for (int j = 0; j < v; ++j) {
            const auto& p = surfaceModel.GetPoints()[i][j]->GetPosition();
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        }
    }

    std::vector<GLuint> idxs;

    for (int i = 0; i < u; ++i) {
        for (int j = 0; j < v - 1; ++j) {
            idxs.push_back(i * v + j);
            idxs.push_back(i * v + j + 1);
        }
    }

    for (int i = 0; i < u - 1; ++i) {
        for (int j = 0; j < v; ++j) {
            idxs.push_back(i * v + j);
            idxs.push_back((i + 1) * v + j);
        }
    }

    mesh->Replace(verts, idxs);
}

void BezierSurfaceView::Draw() {
    glLineWidth(1);
    lineMat->Bind();
    lineMat->SetMat4("Model", glm::mat4(1.0f));
    lineMat->SetMat4("View", Camera::activeCamera->matView);
    lineMat->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->Draw(GL_LINES);

    glPointSize(8);
    pointMat->Bind();
    pointMat->SetMat4("Model", glm::mat4(1.0f));
    pointMat->SetMat4("View", Camera::activeCamera->matView);
    pointMat->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->DrawPartial(0, u * (v - 1) * 2, GL_POINTS);
}
