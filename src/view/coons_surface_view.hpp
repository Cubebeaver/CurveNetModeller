#pragma once

#include <glm/glm.hpp>
#include <model/coons_surface.hpp>
#include <model/bezier_curve.h>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/shader.hpp"

class CoonsSurfaceView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> material;

public:
    CoonsSurfaceView(Shader* sharedShader) {
        std::vector<float> emptyVerts;
        std::vector<GLuint> emptyIdxs;
        mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
        mesh->AddAttribPointer(3, GL_FLOAT, false)
             .AddAttribPointer(3, GL_FLOAT, false)
             .FinishVertexAttribs();

        material = std::make_unique<Material>(sharedShader);
        material->SetVec4("color", glm::vec4(0, 1, 1, 1));
    }

    void Update(const CoonsSurface& curveModel) {
        std::cout << "UPDATE!!!!" << std::endl;

        int res = 20;
        const std::vector<glm::vec3>& points = curveModel.GetRenderPoints(res);
        const std::vector<glm::vec3>& normals = curveModel.GetRenderNormals(res);

        if (points.size() != normals.size()) std::cout << "[-] Na itt valami nagyon félrement ¯\\_(ツ)_/¯" << std::endl;

        std::vector<float> verts;
        verts.reserve(points.size() * 3);

        for (int i = 0; i < points.size(); i++) {
            verts.push_back(points[i].x);
            verts.push_back(points[i].y);
            verts.push_back(points[i].z);

            verts.push_back(normals[i].x);
            verts.push_back(normals[i].y);
            verts.push_back(normals[i].z);
        }

        std::vector<GLuint> idxs;
        idxs.reserve((res - 1) * (res - 1) * 2 * 3);

        for (int i = 0; i < res - 1; i++) {
            for (int j = 0; j < res - 1; j++) {
                // Corners
                int bl = GetIndex(i    , j    , res);
                int br = GetIndex(i + 1, j    , res);
                int tl = GetIndex(i    , j + 1, res);
                int tr = GetIndex(i + 1, j + 1, res);
                // Tri1
                idxs.push_back(bl);
                idxs.push_back(br);
                idxs.push_back(tl);
                // Tri2
                idxs.push_back(tl);
                idxs.push_back(br);
                idxs.push_back(tr);
            }
        }

        mesh->Replace(verts, idxs);
    }

    void Draw() {
        material->Bind();
        material->SetMat4("Model", glm::mat4(1.0f));
        material->SetMat4("View", Camera::activeCamera->matView);
        material->SetMat4("Projection", Camera::activeCamera->matProjection);
        material->SetVec3("color", glm::vec3(0.5, 0.6, 0.7));
        mesh->Draw(GL_TRIANGLES);
    }

private:
    inline int GetIndex(int i, int j, int res) { return i * res + j; }
};
