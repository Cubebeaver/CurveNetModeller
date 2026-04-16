#pragma once
#include <memory>

#include "editor/view/bezier_node_view.hpp"
#include "editor/view/curve_view.hpp"
#include "editor/view/coons_surface_view.hpp"
#include "../../model/object/curve_mesh.h"
#include "../../model/element/i_Node.hpp"
#include "../../model/element/point.h"
#include "editor/workspace/viewport.hpp"
#include "editor/workspace/workspaces.hpp"

class CurveMeshController {
private:
    std::shared_ptr<CurveMesh> curveMesh;

    std::weak_ptr<CoonsSurface> selectedSurface;
    std::weak_ptr<BezierCurve> selectedEdge;
    std::weak_ptr<BezierNode> selectedNode;
    std::weak_ptr<Point> selectedPoint;

    std::vector<std::unique_ptr<BezierNodeView>> nodeViews;
    std::vector<std::unique_ptr<CurveView>> curveViews;
    std::vector<std::unique_ptr<CoonsSurfaceView>> surfaceViews;

    std::weak_ptr<Viewport> vp;

public:
    CurveMeshController() {
        curveMesh = std::make_shared<CurveMesh>();

        curveMesh->CurveMeshChanged += [&]() {
            SyncViews();
        };

        vp = Workspaces::viewport;

        vp.lock()->OnClick += [&] (const glm::vec2& position, ImGuiMouseButton_ button) { OnClick(position, button); };
        vp.lock()->OnDrag += [&] (const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) { OnDrag(totalDelta, delta, position, button); };
    }

    std::shared_ptr<CurveMesh> GetModel() { return curveMesh; }

    void AddNewCurve() {
        auto newEdge = std::make_shared<BezierCurve>();

        newEdge->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), HandleMode::Aligned));
        newEdge->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3( 0.5f,  0.0f, 0.0f), glm::vec3(1.5f, 0.0f, 0.0f), HandleMode::Aligned));

        curveMesh->AddEdge(newEdge);
    }

    void AddExistingCurve(std::shared_ptr<BezierCurve> curve) {
        curveMesh->AddEdge(curve);
    }

    void AddNewSurface() {
        auto c1 = std::make_shared<BezierCurve>();
        c1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.5f, 0.0f, -1.0f), glm::vec3(-0.5f, 0.0f, -1.0f), HandleMode::Aligned));
        c1->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3( 0.5f, 0.0f, -1.0f), glm::vec3( 1.5f, 0.0f, -1.0f), HandleMode::Aligned));

        auto c2 = std::make_shared<BezierCurve>();
        c2->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(-1.5f, 0.0f,  1.0f), glm::vec3(-0.5f, 0.0f,  1.0f), HandleMode::Aligned));
        c2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3( 0.5f, 0.0f,  1.0f), glm::vec3( 1.5f, 0.0f,  1.0f), HandleMode::Aligned));

        auto d1 = std::make_shared<BezierCurve>();
        d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.5f), glm::vec3(-1.0f, 0.0f, -0.5f), HandleMode::Aligned));
        d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(-1.0f, 0.0f,  0.5f), glm::vec3(-1.0f, 0.0f,  1.5f), HandleMode::Aligned));

        auto d2 = std::make_shared<BezierCurve>();
        d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3( 1.0f, 0.0f, -1.5f), glm::vec3( 1.0f, 0.0f, -0.5f), HandleMode::Aligned));
        d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3( 1.0f, 0.0f,  0.5f), glm::vec3( 1.0f, 0.0f,  1.5f), HandleMode::Aligned));

        auto newSurface = std::make_shared<CoonsSurface>(c1, c2, d1, d2);
        curveMesh->AddSurface(newSurface);

        curveMesh->AddEdge(c1);
        curveMesh->AddEdge(c2);
        curveMesh->AddEdge(d1);
        curveMesh->AddEdge(d2);
    }

    void AddExistingSurface(std::shared_ptr<CoonsSurface> surface) {
        curveMesh->AddSurface(surface);
    }

    void Present() {
        // Node views
        for (const auto& nodeView : nodeViews) {
            nodeView->Draw();
        }

        // Curve views
        for (const auto& curveView : curveViews) {
            curveView->Draw();
        }

        // Surface views
        for (const auto& surfaceView : surfaceViews) {
            surfaceView->Draw();
        }
    }

    void ForceSyncViews() {
        SyncViews();
    }

private:
    void SyncViews() {
        // CREATE
        // Node views
        int nodeCount = 0;
        for (const auto& c : curveMesh->GetEdges())
            for (const auto& n : c->GetNodes())
                nodeCount++;

        if (nodeViews.size() != nodeCount) {
            nodeViews.clear();
            for (const auto& modelCurve : curveMesh->GetEdges()) {
                for (const auto& node : modelCurve->GetNodes()) {
                    auto nodeView = std::make_unique<BezierNodeView>();
                    nodeView->Update(*node);
                    nodeViews.push_back(std::move(nodeView));
                }
            }
        }

        // Curve views
        if (curveViews.size() != curveMesh->GetEdges().size()) {
            curveViews.clear();
            for (const auto& curve : curveMesh->GetEdges()) {
                auto nodeView = std::make_unique<CurveView>();
                nodeView->Update(*curve);
                curveViews.push_back(std::move(nodeView));
            }
        }

        // Surface views
        if (surfaceViews.size() != curveMesh->GetSurfaces().size()) {
            surfaceViews.clear();
            for (const auto& surface : curveMesh->GetSurfaces()) {
                auto surfaceView = std::make_unique<CoonsSurfaceView>();
                surfaceView->Update(*surface);
                surfaceViews.push_back(std::move(surfaceView));
            }
        }

        // UPDATE ELEMENTS
        // Node views
        int n = 0;
        for (const auto& modelCurve : curveMesh->GetEdges()) {
            for (const auto& node : modelCurve->GetNodes()) {
                nodeViews[n]->Update(*node);
                n++;
            }
        }

        // Curve views
        auto& edges = curveMesh->GetEdges();
        for (int i = 0; i < edges.size(); i++) {
            curveViews[i]->Update(*edges[i], 10);
        }

        // Surface views
        auto& surfaces = curveMesh->GetSurfaces();
        for (int i = 0; i < surfaces.size(); i++) {
            surfaceViews[i]->Update(*surfaces[i], 10);
        }
    }

private:
    void OnClick(const glm::vec2& position, ImGuiMouseButton_ button) {
        if (button != ImGuiMouseButton_Left) return;

        float closestDistance = std::numeric_limits<float>::max();
        for (const auto& curve : curveMesh->GetEdges()) {
            for (const auto& node : curve->GetNodes()) {
                for (const auto p : node->GetPoints()) {
                    float d = Camera::activeCamera->DistanceToRay(p->GetPosition(), position);
                    if (d < closestDistance) {
                        selectedPoint = p;
                        selectedNode = node;
                        selectedEdge = curve;
                        closestDistance = d;
                    }
                }
            }
        }

        if (closestDistance >= 0.1f) {
            selectedPoint.reset();
            selectedNode.reset();
        }
    }

    void OnDrag(const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) {
        auto p = selectedPoint.lock();
        if (button != ImGuiMouseButton_Left || !p) return;

        // 1. Lekérjük a jelenleg kiválasztott pont/handle 3D-s pozícióját
        std::weak_ptr<Point> currentPos3D = p;

        // --- EZEKET A VÁLTOZÓKAT A KAMERÁBÓL/VIEWPORTBÓL KELL LEKÉRNED ---
        glm::vec3 cameraPos = Camera::activeCamera->position;
        glm::vec3 cameraFront = Camera::activeCamera->direction; // A kamera előre mutató vektora
        glm::mat4 view = Camera::activeCamera->matView;
        glm::mat4 proj = Camera::activeCamera->matProjection;
        // -----------------------------------------------------------------

        // 2. A "Munkasík" definiálása
        // A sík normálvektora a kamera iránya (így a sík párhuzamos a képernyővel)
        glm::vec3 planeNormal = cameraFront;

        // 3. A jelenlegi és az előző egérpozícióhoz tartozó 3D-s sugarak (Rays)
        glm::vec3 currentRayDir = Camera::activeCamera->GetRayDirectionFromScreen(position.x, position.y);
        glm::vec3 prevRayDir = Camera::activeCamera->GetRayDirectionFromScreen(position.x - delta.x, position.y - delta.y);

        // 4. Egy kis lambda függvény, ami kiszámolja a sugár és a munkasík metszéspontját
        auto intersectPlane = [&](glm::vec3 rayDir) -> glm::vec3 {
            float denom = glm::dot(planeNormal, rayDir);
            if (std::abs(denom) > 0.0001f) {
                // A sík távolsága a kamerától a pont irányában
                float t = glm::dot(currentPos3D.lock()->GetPosition() - cameraPos, planeNormal) / denom;
                return cameraPos + rayDir * t;
            }
            return currentPos3D.lock()->GetPosition(); // Fallback
        };

        // 5. A 3D-s elmozdulás kiszámítása
        glm::vec3 hitCurrent = intersectPlane(currentRayDir);
        glm::vec3 hitPrev = intersectPlane(prevRayDir);

        glm::vec3 translation3D = hitCurrent - hitPrev;
        glm::vec3 newPos3D = currentPos3D.lock()->GetPosition() + translation3D;

        // 6. Az új pozíció beállítása a modellben
        p->SetPosition(newPos3D);
    }
};
