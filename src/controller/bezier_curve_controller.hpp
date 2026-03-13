#pragma once

#include<vector>

#include <model/bezier_node.h>
#include <model/bezier_curve.h>
#include <view/bezier_node_view.hpp>
#include <view/bezier_curve_view.hpp>

#include "view/bezier_curve_curvature_comb_view.hpp"
#include "workspace/viewport.hpp"



class BezierCurveController {
private:
    int resolution = 100;

    Viewport* vp;

    std::unique_ptr<BezierCurve> modelCurve;
    
    std::unique_ptr<Shader> sharedShader;
    std::unique_ptr<BezierCurveView> viewCurve;
    std::unique_ptr<BezierCurveCurvatureCombView> curvatureView;
    std::vector<std::unique_ptr<BezierNodeView>> viewNodes;
    
    int selected = -1;
    HandleType selectedPartType = HandleType::None;
public:

    BezierCurveController(Viewport* viewport) : vp(viewport) {
        modelCurve = std::make_unique<BezierCurve>();
        
        //DEBUG
        modelCurve->AddNode(BezierNode(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(-0.7f,  0.3f, 0.0f), glm::vec3(-0.3f,  0.7f, 0.0f)));
        modelCurve->AddNode(BezierNode(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(-0.2f, -0.2f, 0.0f), glm::vec3( 0.2f,  0.2f, 0.0f)));
        modelCurve->AddNode(BezierNode(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.3f, -0.7f, 0.0f), glm::vec3( 0.7f, -0.3f, 0.0f)));
        
        //TODO ezt flyweight-el vagy valamivel megoldani
        sharedShader = std::make_unique<Shader>("resources/shaders/trafo.vert", "resources/shaders/color.frag");
        viewCurve = std::make_unique<BezierCurveView>(sharedShader.get());
        curvatureView = std::make_unique<BezierCurveCurvatureCombView>(sharedShader.get());
        
        SyncViews();

        vp->OnClick += [this] (const glm::vec2& position, ImGuiMouseButton_ button) { this->OnClick(position, button); };
        vp->OnDrag += [this] (const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) { this->OnDrag(totalDelta, delta, position, button); };
    }

    void AddNode(glm::vec3 position) {
        modelCurve->AddNode(BezierNode(position));
        SyncViews();
    }
    void AddNode() {
        if (modelCurve->Nodes.empty()) {
            AddNode(glm::vec3(0, 0, 0));
            return;
        }

        const auto& last = modelCurve->Nodes.back();
        modelCurve->AddNode(BezierNode(last.GetPosition() + glm::vec3(0.1, 0, 0), last.GetLeftHandle() + glm::vec3(0.1, 0, 0), last.GetRightHandle() + glm::vec3(0.1, 0, 0), last.GetMode()));
        SyncViews();
    }

    void RemoveNode() {
        if (modelCurve->Nodes.empty()) return;
        
        modelCurve->RemoveNode(modelCurve->Nodes.back());
        SyncViews();
    }

    void SetNodeMode(HandleMode newMode) {
        if (selected == -1) return;

        modelCurve->Nodes[selected].SetMode(newMode);
        SyncViews();
    }

    void Present() {
        viewCurve->Draw();
        curvatureView->Draw();
        for (int i = 0; i < viewNodes.size(); i++) {
            if (i == selected) viewNodes[i]->Draw(selectedPartType);
            else               viewNodes[i]->Draw();
        }
    }

    // $
    void SyncViews() {
        viewCurve->Update(*modelCurve, resolution);
        curvatureView->Update(*modelCurve, resolution);

        viewNodes.clear();
        for (const auto& node : modelCurve->Nodes) {
            auto nodeView = std::make_unique<BezierNodeView>(sharedShader.get());
            nodeView->Update(node);
            viewNodes.push_back(std::move(nodeView));
        }
    }
private:

    void OnClick(const glm::vec2& position, ImGuiMouseButton_ button) {
        if (modelCurve->Nodes.empty()) return;
        if (button != ImGuiMouseButton_Left) return;
        
        float closestDistance = std::numeric_limits<float>::max();
        for (int i = 0; i < modelCurve->Nodes.size(); i++) {
            const BezierNode& node = modelCurve->Nodes[i];

            const glm::vec3& center = node.GetPosition();
            const glm::vec3& left = node.GetLeftHandle();
            const glm::vec3& right = node.GetRightHandle();

            float centerDistance = Camera::activeCamera->DistanceToRay(center, position);
            float leftDistance = Camera::activeCamera->DistanceToRay(left, position);
            float rightDistance = Camera::activeCamera->DistanceToRay(right, position);

            if (centerDistance < closestDistance) {
                selected = i;
                selectedPartType = HandleType::Center;
                closestDistance = centerDistance;
            }
            if (leftDistance < closestDistance) {
                selected = i;
                selectedPartType = HandleType::Left;
                closestDistance = leftDistance;
            }
            if (rightDistance < closestDistance) {
                selected = i;
                selectedPartType = HandleType::Right;
                closestDistance = rightDistance;
            }
        }

        if (closestDistance >= 0.1f) {
            selected = -1;
            selectedPartType = HandleType::None;
        }
    }

    void OnDrag(const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) {
        if (button != ImGuiMouseButton_Left || selected == -1) return;

        // 1. Lekérjük a jelenleg kiválasztott pont/handle 3D-s pozícióját
        BezierNode& node = modelCurve->Nodes[selected];
        glm::vec3 currentPos3D;
        
        switch(selectedPartType) {
            case HandleType::Center: currentPos3D = node.GetPosition(); break;
            case HandleType::Left:   currentPos3D = node.GetLeftHandle(); break;
            case HandleType::Right:  currentPos3D = node.GetRightHandle(); break;
            default: return;
        }

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
                float t = glm::dot(currentPos3D - cameraPos, planeNormal) / denom;
                return cameraPos + rayDir * t;
            }
            return currentPos3D; // Fallback
        };

        // 5. A 3D-s elmozdulás kiszámítása
        glm::vec3 hitCurrent = intersectPlane(currentRayDir);
        glm::vec3 hitPrev = intersectPlane(prevRayDir);
        
        glm::vec3 translation3D = hitCurrent - hitPrev;
        glm::vec3 newPos3D = currentPos3D + translation3D;

        // 6. Az új pozíció beállítása a modellben
        switch(selectedPartType) {
            case HandleType::Center: node.SetPosition(newPos3D); break;
            case HandleType::Left:   node.SetLeftHandle(newPos3D); break;
            case HandleType::Right:  node.SetRightHandle(newPos3D); break;
            default: break;
        }
        
        // (Ha már bekerült az OnCurveChanged esemény a Modellbe, ezt a sort törölheted!)
        SyncViews();
    }
};