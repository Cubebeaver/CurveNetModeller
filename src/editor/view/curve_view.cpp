#include "curve_view.h"

CurveView::CurveView() {
    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

    material = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    material->SetVec4("color", glm::vec4(0, 1, 1, 1));
}

void CurveView::Update(const ICurve& curveModel, int resolution) {
    const auto* spline = dynamic_cast<const ISpline*>(&curveModel);
    int res = resolution;
    if (spline) res *= spline->GetSegmentCount();

    const std::vector<glm::vec3>& points = RenderPointGenerator::GetRenderPoints(curveModel, res);

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

void CurveView::Draw() {
    glLineWidth(2);
    material->Bind();
    material->SetMat4("Model", glm::mat4(1.0f));
    material->SetMat4("View", Camera::activeCamera->matView);
    //material->SetMat4("View", glm::mat4(1.0f));
    material->SetMat4("Projection", Camera::activeCamera->matProjection);
    //material->SetMat4("Projection", glm::mat4(1.0f));
    mesh->Draw(GL_LINE_STRIP);
}