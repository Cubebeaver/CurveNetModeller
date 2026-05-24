#include "surface_view.h"

using namespace gl_engine;

int GetIndex(int i, int j, int res) { return i * res + j; }

SurfaceView::SurfaceView() {
    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false)
         .AddAttribPointer(3, GL_FLOAT, false)
         .FinishVertexAttribs();

    material = std::make_unique<Material>(SharedShaders::Get("shaded"));
    material->SetVec4("color", glm::vec4(.1f, .2f, .3f, 1));
}

void SurfaceView::Update(const ISurface& surfaceModel, int resolution) {
    const std::vector<glm::vec3>& points = GenRenderPoints(surfaceModel, resolution);
    const std::vector<glm::vec3>& normals = GenRenderNormals(surfaceModel, resolution);

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

void SurfaceView::Draw() {
    material->Bind();
    material->SetMat4("Model", glm::mat4(1.0f));
    material->SetMat4("View", Camera::activeCamera->matView);
    material->SetMat4("Projection", Camera::activeCamera->matProjection);
    mesh->Draw(GL_TRIANGLES);
}

std::vector<glm::vec3> SurfaceView::GenRenderPoints(const ISurface& surface, int resolution) {
    std::vector<glm::vec3> surfacePoints;
    surfacePoints.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float u = static_cast<float>(i) / (resolution - 1);
            float v = static_cast<float>(j) / (resolution - 1);

            surfacePoints.push_back(surface.Evaluate(u, v));
        }
    }

    return surfacePoints;
}

std::vector<glm::vec3> SurfaceView::GenRenderNormals(const ISurface& surface, int resolution) {
    std::vector<glm::vec3> surfaceNormals;
    surfaceNormals.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float u = static_cast<float>(i) / (resolution - 1);
            float v = static_cast<float>(j) / (resolution - 1);

            glm::vec3 normal = surface.EvaluateNormal(u, v);
            surfaceNormals.push_back(normal);
        }
    }
    return surfaceNormals;
}
