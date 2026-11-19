#include "curve_rotation_minimizing_frame_view.h"

using namespace gl_engine;

CurveRotationMinimizingFrameView::CurveRotationMinimizingFrameView(std::shared_ptr<ICurve> c, bool drawSpace)
    : curveModel(c), drawSpace(drawSpace) {
    rmf = std::make_unique<RotationMinimizingFrame>(c, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    c->CurveChanged.AddListener(this, &CurveRotationMinimizingFrameView::OnChanged);

    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false).FinishVertexAttribs();

    // Reference normal
    materialR = std::make_unique<Material>(SharedShaders::Get("solid_color"));
    materialR->SetVec4("color", glm::vec4(0, 0.75f, 0, 1));
    if (drawSpace) {
        // Tangent
        materialT = std::make_unique<Material>(SharedShaders::Get("solid_color"));
        materialT->SetVec4("color", glm::vec4(0.75f, 0, 0, 1));
        // Binormal (Cross boundary derivate)
        materialS = std::make_unique<Material>(SharedShaders::Get("solid_color"));
        materialS->SetVec4("color", glm::vec4(0, 0, 0.75f, 1));
    }

}

void CurveRotationMinimizingFrameView::Update(int resolution, float length) {
    const std::vector<RMFFrame> frames = rmf->GenerateFrames(resolution);

    std::vector<float> verts;
    std::vector<GLuint> idxs;


    if (!drawSpace) {
        verts.reserve(frames.size() * 3 * 2);
        idxs.reserve(frames.size() * 2);
    } else /* if drawSpace */ {
        verts.reserve(frames.size() * 3 * 4);
        idxs.reserve(frames.size() * 6);
    }


    for (const auto & frame : frames) {
        verts.push_back(frame.position.x);
        verts.push_back(frame.position.y);
        verts.push_back(frame.position.z);
    }
    // Reference normal
    for (const auto & frame : frames) {
        verts.push_back(frame.position.x + frame.r.x * length);
        verts.push_back(frame.position.y + frame.r.y * length);
        verts.push_back(frame.position.z + frame.r.z * length);
    }
    if (drawSpace) {
        // Tangent
        for (const auto & frame : frames) {
            verts.push_back(frame.position.x + frame.t.x * length);
            verts.push_back(frame.position.y + frame.t.y * length);
            verts.push_back(frame.position.z + frame.t.z * length);
        }
        // Binormal (Cross boundary derivate)
        for (const auto & frame : frames) {
            verts.push_back(frame.position.x + frame.s.x * length);
            verts.push_back(frame.position.y + frame.s.y * length);
            verts.push_back(frame.position.z + frame.s.z * length);
        }
    }

    // Reference normal
    for (int i = 0; i < frames.size(); i++) {
        idxs.push_back(i);
        idxs.push_back(i + frames.size());
    }
    if (drawSpace) {
        // Tangent
        for (int i = 0; i < frames.size(); i++) {
            idxs.push_back(i);
            idxs.push_back(i + 2 * frames.size());
        }
        // Binormal (Cross boundary derivate)
        for (int i = 0; i < frames.size(); i++) {
            idxs.push_back(i);
            idxs.push_back(i + 3 * frames.size());
        }
    }

    mesh->Replace(verts, idxs);
    lastResolution = resolution;
}

void CurveRotationMinimizingFrameView::Draw() {
    glLineWidth(1);
    materialR->Bind();
    materialR->SetMat4("Model", glm::mat4(1.0f));
    materialR->SetMat4("View", Camera::activeCamera->matView);
    materialR->SetMat4("Projection", Camera::activeCamera->matProjection);

    mesh->DrawPartial(0, 2 * lastResolution, GL_LINES);
    if (drawSpace) {
        materialT->Bind();
        materialT->SetMat4("Model", glm::mat4(1.0f));
        materialT->SetMat4("View", Camera::activeCamera->matView);
        materialT->SetMat4("Projection", Camera::activeCamera->matProjection);
        mesh->DrawPartial(2 * lastResolution, 4 * lastResolution, GL_LINES);
        materialS->Bind();
        materialS->SetMat4("Model", glm::mat4(1.0f));
        materialS->SetMat4("View", Camera::activeCamera->matView);
        materialS->SetMat4("Projection", Camera::activeCamera->matProjection);
        mesh->DrawPartial(4 * lastResolution, 6 * lastResolution, GL_LINES);
    }
}