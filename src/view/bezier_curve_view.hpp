#pragma once

#include <glm/glm.hpp>
#include "model/bezier_curve.h"

#include "gl_engine/gl_engine.h"

class BezierCurveView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> material;

public:
    BezierCurveView(Shader* sharedShader) {
        std::vector<float> emptyVerts;
        std::vector<GLuint> emptyIdxs;
        mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
        mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

        material = std::make_unique<Material>(sharedShader);
        material->SetVec4("color", glm::vec4(0, 1, 1, 1));
    }

    void Update(const BezierCurve& curveModel) {
        const std::vector<glm::vec3>& points = curveModel.GenerateRenderPoints(20);

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
        mesh->Draw(GL_LINE_STRIP);
    }
};