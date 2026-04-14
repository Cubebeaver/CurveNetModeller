#include  "coons_surface.h"

CoonsSurface::CoonsSurface(
    std::shared_ptr<BezierCurve> c1, std::shared_ptr<BezierCurve> c2,
    std::shared_ptr<BezierCurve> d1, std::shared_ptr<BezierCurve> d2
) : c1(c1), c2(c2), d1(d1), d2(d2) {
    this->c1->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
    this->c2->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
    this->d1->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
    this->d2->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
}

glm::vec3 CoonsSurface::Evaluate(float u, float v) const {
    const auto& S_u0 = c1->EvaluatePosition(u);
    const auto& S_u1 = c2->EvaluatePosition(u);
    const auto& S_0v = d1->EvaluatePosition(v);
    const auto& S_1v = d2->EvaluatePosition(v);

    const auto& S1_uv = (1 - v) * S_u0 + v * S_u1;
    const auto& S2_uv = (1 - u) * S_0v + u * S_1v;

    const auto& S_00 = c1->EvaluatePosition(0);
    const auto& S_10 = c1->EvaluatePosition(1);
    const auto& S_01 = c2->EvaluatePosition(0);
    const auto& S_11 = c2->EvaluatePosition(1);

    const auto& S12_uv = (1 - v) * ((1 - u) * S_00 + u * S_10)
                             + v * ((1 - u) * S_01 + u * S_11);

    return S1_uv + S2_uv - S12_uv;
}

glm::vec3 CoonsSurface::EvaluateNormal(float u, float v) const {
    const float dx = 0.01f;

    glm::vec3 c = Evaluate(u, v);
    glm::vec3 du = Evaluate(u + dx, v);
    glm::vec3 dv = Evaluate(u, v + dx);

    glm::vec3 du_dx = du - c;
    glm::vec3 dv_dx = dv - c;

    glm::vec3 normal = glm::normalize(glm::cross(dv_dx, du_dx));
    return normal;
}

std::vector<glm::vec3> CoonsSurface::GetRenderPoints(int resolution) const {
    std::vector<glm::vec3> surfacePoints;
    surfacePoints.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            surfacePoints.push_back(Evaluate((float)i / (resolution - 1), (float)j / (resolution - 1)));
        }
    }
    return surfacePoints;
}

std::vector<glm::vec3> CoonsSurface::GetRenderNormals(int resolution) const {
    std::vector<glm::vec3> surfaceNormals;
    surfaceNormals.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float u = (float)i / (resolution - 1);
            float v = (float)j / (resolution - 1);

            glm::vec3 normal = EvaluateNormal(u, v);
            surfaceNormals.push_back(normal);
        }
    }
    return surfaceNormals;
}