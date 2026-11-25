#include "editor/view/n_sided_surface_view.h"
#include "util/console_helper.hpp"

using namespace gl_engine;

NSidedSurfaceView::NSidedSurfaceView() {
    std::vector<float> emptyVerts;
    std::vector<GLuint> emptyIdxs;
    mesh = std::make_unique<Mesh>(emptyVerts, emptyIdxs);
    mesh->AddAttribPointer(3, GL_FLOAT, false)
         .AddAttribPointer(3, GL_FLOAT, false)
         .FinishVertexAttribs();

    material = std::make_unique<Material>(SharedShaders::Get("zebra"));
    material->SetFloat("stripeFrequency", 20);
    material->SetFloat("stripeSharpness", 0.01f);
}

void NSidedSurfaceView::Update(const NSidedSurface& surfaceModel, int resolution) {
    const std::vector<glm::vec3>& points = GenRenderPoints(surfaceModel, resolution);
    const std::vector<glm::vec3>& normals = GenRenderNormals(surfaceModel, resolution);
    const int res = resolution - 1;
    const int N = surfaceModel.EdgeCount();

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

    std::vector<GLuint> indices;
    indices.reserve((res * (res + 1) / 2) * 3 + ((res) * (res - 1) / 2) * 3);
    const int numVertsPerPartition = res * (res + 1) / 2;

    for (int sideIdx = 0; sideIdx < N; sideIdx++) {
        const int partitionOffset = sideIdx * numVertsPerPartition + 1;
        const int nextPartitionOffset = ((sideIdx + 1) % N) * numVertsPerPartition + 1;
        int bankStart = 0;
        int nextBankStart = res;
        for (int tIdx = 0; tIdx < res; tIdx++) {

            for (int sIdx = 0; sIdx < res - tIdx; sIdx++) {

                // A közepénél vagyunk
                if (tIdx == res - 1 /* && sIdx == res - tIdx - 1 */) { // sIdx = res - tIdx - 1 <-- ennek mindig teljesülnie kellene
                    indices.push_back(partitionOffset + bankStart /* + sIdx */); // sIdx <-- ennek mindig 0-nak kellene lennie
                    indices.push_back(nextPartitionOffset + bankStart);
                    indices.push_back(0); // Közepe
                }
                // A következő partíció határánál vagyunk
                else if (sIdx == res - tIdx - 1) {
                    indices.push_back(partitionOffset + bankStart + sIdx);
                    indices.push_back(nextPartitionOffset + bankStart);
                    indices.push_back(nextPartitionOffset + nextBankStart);
                }
                // Normális OK.
                else {
                    indices.push_back(partitionOffset + bankStart + sIdx);
                    indices.push_back(partitionOffset + bankStart + sIdx + 1);
                    indices.push_back(partitionOffset + nextBankStart + sIdx);

                    indices.push_back(partitionOffset + bankStart + sIdx + 1);
                    if (sIdx == res - tIdx - 2) indices.push_back(nextPartitionOffset + nextBankStart);
                    else /*   Super normal   */ indices.push_back(partitionOffset + nextBankStart + sIdx + 1);
                    indices.push_back(partitionOffset + nextBankStart + sIdx);
                }
            }

            bankStart += res - tIdx;
            nextBankStart += res - tIdx - 1;
        }
    }

    mesh->Replace(verts, indices);
}

void NSidedSurfaceView::Draw() {
    material->Bind();
    material->SetMat4("Model", glm::mat4(1.0f));
    material->SetMat4("View", Camera::activeCamera->matView);
    material->SetMat4("Projection", Camera::activeCamera->matProjection);
    material->SetVec3("cameraPos", Camera::activeCamera->position);
    mesh->Draw(GL_TRIANGLES);
}

std::vector<glm::vec3> NSidedSurfaceView::GenRenderPoints(const NSidedSurface& surface, int resolution) {
    std::vector<glm::vec3> surfacePoints;
    int res = resolution - 1;

    const int N = surface.EdgeCount();
    constexpr glm::vec2 center(0.5f, 0.5f);
    const std::vector<glm::vec2>& domainPoints = surface.GetDomainVertices();

    surfacePoints.push_back(surface.Evaluate(center.x, center.y));

    for (int sideIdx = 0; sideIdx < N; sideIdx++) {
        const glm::vec2 edgeStart = domainPoints[sideIdx];
        const glm::vec2 edgeEnd   = domainPoints[(sideIdx + 1) % N];

        for (int tIdx = 0; tIdx < res; tIdx ++) {
            const float t = static_cast<float>(tIdx) / static_cast<float>(res);

            for (int sIdx = 0; sIdx < res - tIdx; sIdx++) {
                const float s = static_cast<float>(sIdx) / static_cast<float>(res);

                const glm::vec2 domainParam = glm::mix(glm::mix(edgeStart, edgeEnd, s), center, t);
                surfacePoints.push_back(surface.Evaluate(domainParam.x, domainParam.y));
            }
        }
    }

    return surfacePoints;
}

std::vector<glm::vec3> NSidedSurfaceView::GenRenderNormals(const NSidedSurface& surface, int resolution) {
    std::vector<glm::vec3> surfaceNormals;
    int res = resolution - 1;

    const int N = surface.EdgeCount();
    constexpr glm::vec2 center(0.5f, 0.5f);
    const std::vector<glm::vec2>& domainPoints = surface.GetDomainVertices();

    surfaceNormals.push_back(surface.Evaluate(center.x, center.y));

    for (int sideIdx = 0; sideIdx < N; sideIdx++) {
        const glm::vec2 edgeStart = domainPoints[sideIdx];
        const glm::vec2 edgeEnd   = domainPoints[(sideIdx + 1) % N];

        for (int tIdx = 0; tIdx < res; tIdx ++) {
            const float t = static_cast<float>(tIdx) / static_cast<float>(res);

            for (int sIdx = 0; sIdx < res - tIdx; sIdx++) {
                const float s = static_cast<float>(sIdx) / static_cast<float>(res);

                const glm::vec2 domainParam = glm::mix(glm::mix(edgeStart, edgeEnd, s), center, t);
                surfaceNormals.push_back(surface.EvaluateNormal(domainParam.x, domainParam.y));
            }
        }
    }

    return surfaceNormals;
}