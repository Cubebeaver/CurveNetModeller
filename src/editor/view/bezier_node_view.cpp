#include "bezier_node_view.h"

BezierNodeView::BezierNodeView() {
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

void BezierNodeView::Update(const BezierNode& nodeModel) {
    std::vector<float> vertices {
        nodeModel.GetLeftHandle()->GetPosition().x,   nodeModel.GetLeftHandle()->GetPosition().y,   nodeModel.GetLeftHandle()->GetPosition().z,
        nodeModel.GetCenterHandle()->GetPosition().x, nodeModel.GetCenterHandle()->GetPosition().y, nodeModel.GetCenterHandle()->GetPosition().z,
        nodeModel.GetRightHandle()->GetPosition().x,  nodeModel.GetRightHandle()->GetPosition().y,  nodeModel.GetRightHandle()->GetPosition().z,
    };
    std::vector<GLuint> indices { 0, 1, 2 };

    mesh->Replace(vertices, indices);
}

void BezierNodeView::Draw(BezierHandleType selected) {
    glLineWidth(1);
    lineMat->Bind();
    lineMat->SetMat4("Model", glm::mat4(1.0f));
    lineMat->SetMat4("View", Camera::activeCamera->matView);
    lineMat->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->Draw(GL_LINE_STRIP);

    glPointSize(8);
    pointMat->Bind();
    pointMat->SetMat4("Model", glm::mat4(1.0f));
    pointMat->SetMat4("View", Camera::activeCamera->matView);
    pointMat->SetMat4("Projection", Camera::activeCamera->matProjection);

    selectedMat->Bind();
    selectedMat->SetMat4("Model", glm::mat4(1.0f));
    selectedMat->SetMat4("View", Camera::activeCamera->matView);
    selectedMat->SetMat4("Projection", Camera::activeCamera->matProjection);

    if (static_cast<int>(selected) & static_cast<int>(BezierHandleType::Left)) selectedMat->Bind();
    else /* Left not selected */                                               pointMat->Bind();
    mesh->DrawPartial(0, 1, GL_POINTS);

    if (static_cast<int>(selected) & static_cast<int>(BezierHandleType::Center)) selectedMat->Bind();
    else /* Center not selected */                                               pointMat->Bind();
    mesh->DrawPartial(1, 2, GL_POINTS);

    if (static_cast<int>(selected) & static_cast<int>(BezierHandleType::Right)) selectedMat->Bind();
    else /* Right not selected */                                               pointMat->Bind();
    mesh->DrawPartial(2, 3, GL_POINTS);
}