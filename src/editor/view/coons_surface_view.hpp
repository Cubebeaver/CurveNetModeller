#pragma once

#include <glm/glm.hpp>
#include <model/element/coons_surface.h>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"

class CoonsSurfaceView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> material;

public:
    CoonsSurfaceView() {
        std::vector<float> emptyVerts;
        std::vector<GLuint> emptyIdxs;
        mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
        mesh->AddAttribPointer(3, GL_FLOAT, false)
             .AddAttribPointer(3, GL_FLOAT, false)
             .FinishVertexAttribs();

        material = std::make_unique<Material>(SharedShaders::Get("shaded"));
        material->SetVec4("color", glm::vec4(.1f, .2f, .3f, 1));
    }

    void Update(const CoonsSurface& surfaceModel, int resolution = 20) {
        const std::vector<glm::vec3>& points = surfaceModel.GetRenderPoints(resolution);
        const std::vector<glm::vec3>& normals = surfaceModel.GetRenderNormals(resolution);

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
        idxs.reserve((resolution - 1) * (resolution - 1) * 2 * 3);

        for (int i = 0; i < resolution - 1; i++) {
            for (int j = 0; j < resolution - 1; j++) {
                // Corners
                int bl = GetIndex(i    , j    , resolution);
                int br = GetIndex(i + 1, j    , resolution);
                int tl = GetIndex(i    , j + 1, resolution);
                int tr = GetIndex(i + 1, j + 1, resolution);
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
        mesh->Draw(GL_TRIANGLES);
    }

private:
    inline int GetIndex(int i, int j, int res) { return i * res + j; }
};
