#pragma once

#include <glm/glm.hpp>
#include "model/bezier_curve.h"

#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/camera.hpp"
#include "gl_engine/shared_shaders.hpp"
using namespace gl_engine;

class CurveView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> material;

public:
    CurveView() {
        std::vector<float> emptyVerts;
        std::vector<GLuint> emptyIdxs;
        mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
        mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

        material = std::make_unique<Material>(SharedShaders::Get("solid_color"));
        material->SetVec4("color", glm::vec4(0, 1, 1, 1));
    }

    void Update(const ICurve& curveModel, int resolution = 50) {


        const std::vector<glm::vec3>& points = GetRenderPoints(curveModel, resolution);

        std::vector<float> verts; 
        verts.reserve(points.size() * 3);
        std::vector<GLuint> idxs; 
        idxs.reserve(points.size());

        for (int i = 0; i < points.size(); i++) {
            verts.push_back(points[i].x);
            verts.push_back(points[i].y);
            verts.push_back(points[i].z);
            idxs.push_back(i);
        }
        
        mesh->Replace(verts, idxs);
    }

    void Draw() {
        glLineWidth(2);
        material->Bind();
        material->SetMat4("Model", glm::mat4(1.0f));
        material->SetMat4("View", Camera::activeCamera->matView);
        //material->SetMat4("View", glm::mat4(1.0f));
        material->SetMat4("Projection", Camera::activeCamera->matProjection);
        //material->SetMat4("Projection", glm::mat4(1.0f));
        mesh->Draw(GL_LINE_STRIP);
    }

    static std::vector<glm::vec3> GetRenderPoints(const ICurve& curveModel, int resolution) {
        std::vector<glm::vec3> renderPoints;

        // int segments = GetSegmentCount();
        // if (segments == 0 && !Nodes.empty()) {
        //     renderPoints.push_back(Nodes[0]->GetCenterHandle()->GetPosition());
        //     return renderPoints;
        // }

        renderPoints.reserve(resolution);

        // for (int i = 0; i < segments; ++i) {
        //     int steps = (i == segments - 1) ? resolution : resolution - 1;
        //
        //     for (int step = 0; step <= steps; ++step) {
        //         float t = static_cast<float>(step) / static_cast<float>(resolution);
        //         renderPoints.push_back(EvaluateSegment(i, t));
        //     }
        // }

        for (int i = 0; i < resolution; i++) {
            float t = (float)i / (float)resolution;
            renderPoints.push_back(curveModel.EvaluatePosition(t));
        }

        return renderPoints;
    }
};
