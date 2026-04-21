#pragma once

#include<vector>

#include <model/element/bezier_node.h>
#include <model/element/bezier_curve.h>

#include "gl_engine/camera.hpp"
#include <editor/view/bezier_node_view.h>
#include <editor/view/curve_view.h>

#include "editor/view/curve_curvature_comb_view.h"
#include "editor/workspace/viewport.hpp"
#include "editor/workspace/workspaces.hpp"


//TODO Nem tudom miért de miután áttértem smart pointerekre (nem fix hogy ez az oka) utána,
//     ha az utosó kontrol pontot is kitöröm, akkor valahogy mefagy az egész, de pörgeti a processzort,
//     de nem száll el se stack overflow, se sigsegv-vel ugyh nem tudom mi lehet
class BezierCurveController {
private:
    int resolution = 20;
    float length = 1.0f;
    bool showCurvature = false;

    std::weak_ptr<Viewport> vp;

    std::shared_ptr<BezierCurve> modelCurve;

    std::unique_ptr<CurveView> viewCurve;
    std::unique_ptr<CurveCurvatureCombView> curvatureView;
    std::vector<std::unique_ptr<BezierNodeView>> viewNodes;

    std::weak_ptr<Point> selectedPoint;
    std::weak_ptr<BezierNode> selectedNode;
public:

    BezierCurveController() {
        vp = Workspaces::viewport;

        modelCurve = std::make_shared<BezierCurve>();
        
        //TODO ezt flyweight-el vagy valamivel megoldani
        viewCurve = std::make_unique<CurveView>();
        curvatureView = std::make_unique<CurveCurvatureCombView>();

        modelCurve->CurveChanged.AddListener(this, &BezierCurveController::SyncViews);

        vp.lock()->OnClick.AddListener(this, &BezierCurveController::OnClick);
        vp.lock()->OnDrag.AddListener(this, &BezierCurveController::OnDrag);
    }

    std::shared_ptr<BezierCurve> GetModel() { return modelCurve; }

    void AddNode(std::shared_ptr<BezierNode> node) {
        modelCurve->AddNode(node);
    }
    void AddNode() {
        if (modelCurve->GetNodes().empty()) {
            AddNode(std::make_shared<BezierNode>(glm::vec3(0, 0, 0)));
            return;
        }

        const auto& last = modelCurve->GetNodes().back();
        modelCurve->AddNode(std::make_shared<BezierNode>(
            last->GetCenterHandle()->GetPosition() + glm::vec3(1, 0, 0),
            last->GetLeftHandle()->GetPosition() + glm::vec3(0, 0, 0),
            last->GetRightHandle()->GetPosition() + glm::vec3(0, 0, 0),
            last->GetMode())
        );
    }

    void RemoveNode() {
        if (modelCurve->GetNodes().empty()) return;
        int idx = modelCurve->IndexOf(selectedNode);

        modelCurve->RemoveNode(selectedNode);

        if (idx >= modelCurve->GetNodes().size()) idx = modelCurve->GetNodes().size() - 1;
        if (idx < 0) return;
        selectedNode = modelCurve->GetNodes()[idx];
        selectedPoint = selectedNode.lock()->GetCenterHandle();
    }

    void SetNodeMode(HandleMode newMode) {
        auto n = selectedNode.lock();
        if (!n) return;

        n->SetMode(newMode);
    }

    void SetNormalType(bool normalType) {
        curvatureView->SetNormalType(normalType);
        SyncViews();
    }
    void SetCurvatureVisibility(bool visible) {
        showCurvature = visible;
    }
    void SetResolution(int newResolution) {
        if (resolution == newResolution) return;
        resolution = newResolution;
        SyncViews();
    }

    void Present() {
        viewCurve->Draw();
        if (showCurvature) curvatureView->Draw();

        int idx = modelCurve->IndexOf(selectedNode);
        BezierHandleType selectedPartType = selectedNode.lock()->GetHandleType(selectedPoint);

        for (int i = 0; i < viewNodes.size(); i++) {
            if (i == idx) viewNodes[i]->Draw(selectedPartType);
            else          viewNodes[i]->Draw();
        }
    }

    // $
    void SyncViews() {
        viewCurve->Update(*modelCurve, resolution);
        if (showCurvature) curvatureView->Update(*modelCurve, resolution, length);

        viewNodes.clear();
        for (const auto& node : modelCurve->GetNodes()) {
            auto nodeView = std::make_unique<BezierNodeView>();
            nodeView->Update(*node);
            viewNodes.push_back(std::move(nodeView));
        }
    }

    void SetCombLength(float length) {
        this->length = length;
        curvatureView->Update(*modelCurve, resolution, length);
    }

private:

    void OnClick(const glm::vec2& position, ImGuiMouseButton_ button) {
        if (modelCurve->GetNodes().empty()) return;
        if (button != ImGuiMouseButton_Left) return;
        
        float closestDistance = std::numeric_limits<float>::max();
        for (int i = 0; i < modelCurve->GetNodes().size(); i++) {
            auto node = modelCurve->GetNodes()[i];

            const auto& points = node->GetPoints();
            for (const auto p : points) {
                float d = Camera::activeCamera->DistanceToRay(p->GetPosition(), position);
                if (d < closestDistance) {
                    selectedPoint = p;
                    selectedNode = node;
                    closestDistance = d;
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