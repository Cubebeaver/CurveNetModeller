#include "curve_curvature_comb_view.h"

using namespace gl_engine;

CurveCurvatureCombView::CurveCurvatureCombView() {
    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

    material = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    material->SetVec4("color", glm::vec4(0, 1, 0.5f, 1));
}

void CurveCurvatureCombView::SetNormalType(bool normalType) {
    this->normalType = normalType;
}

void CurveCurvatureCombView::Update(const BezierCurve& curveModel, int resolution, float length) {
    const std::vector<glm::vec3>& points = curveModel.GenerateRenderPoints(resolution);
    const std::vector<float>& curvatures = curveModel.GenerateRenderCurvatures(resolution);
    std::vector<glm::vec3> normals;
    if (normalType) {
        normals = curveModel.GenerateRenderCameraNormals(resolution, Camera::activeCamera->direction);
    } else {
        normals = curveModel.GenerateRenderNormals(resolution);
    }

    std::vector<float> verts;
    verts.reserve(points.size() * 3 * 2);
    std::vector<GLuint> idxs;
    idxs.reserve(points.size() * 2);

    for (int i = 0; i < points.size(); i++) {
        verts.push_back(points[i].x);
        verts.push_back(points[i].y);
        verts.push_back(points[i].z);
    }
    for (int i = 0; i < points.size(); i++) {
        verts.push_back(points[i].x + -normals[i].x * curvatures[i] * length);
        verts.push_back(points[i].y + -normals[i].y * curvatures[i] * length);
        verts.push_back(points[i].z + -normals[i].z * curvatures[i] * length);
    }

    for (int i = 0; i < points.size(); i++) {
        idxs.push_back(i);
        idxs.push_back(i + points.size());
    }
    for (int i = points.size(); i < (points.size() == 0 ? 0 : points.size() * 2 - 1); i++) {
        idxs.push_back(i);
        idxs.push_back(i + 1);
    }

    mesh->Replace(verts, idxs);
}

void CurveCurvatureCombView::Draw() {
    glLineWidth(1);
    material->Bind();
    material->SetMat4("Model", glm::mat4(1.0f));
    material->SetMat4("View", Camera::activeCamera->matView);
    material->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->Draw(GL_LINES);
}