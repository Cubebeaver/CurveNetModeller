#include "curve_rotation_minimizing_frame_view.h"

using namespace gl_engine;

CurveRotationMinimizingFrameView::CurveRotationMinimizingFrameView(std::shared_ptr<BezierCurve> c)
    : curveModel(c) {
    rmf = std::make_unique<RotationMinimizingFrame>(c, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    c->CurveChanged.AddListener(this, &CurveRotationMinimizingFrameView::OnChanged);

    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

    material = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    material->SetVec4("color", glm::vec4(0, 1, 0.5f, 1));
}

std::ostream& operator << (std::ostream& lhs, const glm::vec3& rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    return lhs;
}

void CurveRotationMinimizingFrameView::Update(int resolution, float length) {
    const std::vector<RMFFrame> frames = rmf->GenerateFrames(resolution);

    std::vector<float> verts;
    verts.reserve(frames.size() * 3 * 2);
    std::vector<GLuint> idxs;
    idxs.reserve(frames.size() * 2);

    for (const auto & frame : frames) {
        verts.push_back(frame.position.x);
        verts.push_back(frame.position.y);
        verts.push_back(frame.position.z);
    }
    for (const auto & frame : frames) {
        verts.push_back(frame.position.x + frame.r.x * length);
        verts.push_back(frame.position.y + frame.r.y * length);
        verts.push_back(frame.position.z + frame.r.z * length);
    }

    for (int i = 0; i < frames.size(); i++) {
        idxs.push_back(i);
        idxs.push_back(i + frames.size());
    }

    mesh->Replace(verts, idxs);
}

void CurveRotationMinimizingFrameView::Draw() {
    glLineWidth(1);
    material->Bind();
    material->SetMat4("Model", glm::mat4(1.0f));
    material->SetMat4("View", Camera::activeCamera->matView);
    material->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->Draw(GL_LINES);
}