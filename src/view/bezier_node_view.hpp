#pragma once

#include "model/bezier_node.h"

#include "gl_engine/mesh.hpp"

class BezierNodeView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> lineMat;
    std::unique_ptr<Material> pointMat;

public:
    BezierNodeView(Shader* sharedShader) {
        std::vector<float> emptyVerts;
        std::vector<GLuint> emptyIdxs;
        mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
        mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();
        
        lineMat = std::make_unique<Material>(sharedShader);
        lineMat->SetVec4("color", glm::vec4(1, 1, 0, 1));
        
        pointMat = std::make_unique<Material>(sharedShader);
        pointMat->SetVec4("color", glm::vec4(1, 0, 0, 1));
    }

    void Update(const BezierNode& nodeModel) {
        std::vector<float> vertices {
            nodeModel.GetLeftHandle().x,  nodeModel.GetLeftHandle().y,  nodeModel.GetLeftHandle().z,
            nodeModel.GetPosition().x,    nodeModel.GetPosition().y,    nodeModel.GetPosition().z,
            nodeModel.GetRightHandle().x, nodeModel.GetRightHandle().y, nodeModel.GetRightHandle().z,
        };
        std::vector<GLuint> indices { 0, 1, 2 };
        
        mesh->Replace(vertices, indices);
    }

    void Draw() {
        glLineWidth(1);
        lineMat->Bind();
        mesh->Draw(GL_LINE_STRIP);
        
        glPointSize(8);
        pointMat->Bind();
        mesh->Draw(GL_POINTS);
    }
};