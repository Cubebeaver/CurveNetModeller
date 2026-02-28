#pragma once

#include<vector>

#include <model/bezier_node.h>
#include <model/bezier_curve.h>
#include <view/bezier_node_view.hpp>
#include <view/bezier_curve_view.hpp>

#include "workspace/viewport.hpp"






class BezierCurveController {
private:
    Viewport* vp;

    std::unique_ptr<BezierCurve> modelCurve;
    
    std::unique_ptr<Shader> sharedShader;
    std::unique_ptr<BezierCurveView> viewCurve;
    std::vector<std::unique_ptr<BezierNodeView>> viewNodes;
    
    BezierNode* selected = nullptr;
    HandleType selectedPartType = HandleType::None;
public:

    BezierCurveController(Viewport* viewport) : vp(viewport) {
        modelCurve = std::make_unique<BezierCurve>();
        
        //DEBUG
        modelCurve->AddNode(BezierNode(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(-0.7f,  0.3f, 0.0f), glm::vec3(-0.3f,  0.7f, 0.0f)));
        modelCurve->AddNode(BezierNode(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec3(-0.2f, -0.2f, 0.0f), glm::vec3( 0.2f,  0.2f, 0.0f)));
        modelCurve->AddNode(BezierNode(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.3f, -0.7f, 0.0f), glm::vec3( 0.7f, -0.3f, 0.0f)));
        
        //TODO ezt flyweight-el vagy valamivel megoldani
        sharedShader = std::make_unique<Shader>("resources/shaders/color.vert", "resources/shaders/color.frag");
        viewCurve = std::make_unique<BezierCurveView>(sharedShader.get());
        
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
        if (!selected) return;

        selected->SetMode(newMode);
        SyncViews();
    }

    void Present() {
        viewCurve->Draw();
        for (auto& nodeView : viewNodes) {
            nodeView->Draw();
        }
    }

private:
    glm::vec2 CursorToScreen(glm::vec2 cursor) {
        const auto& flipped = glm::vec2(cursor.x, vp->viewportBuffer->Height - cursor.y);
        const auto& size = glm::vec2(vp->viewportBuffer->Width, vp->viewportBuffer->Height);
        return flipped / size * glm::vec2(2, 2) - glm::vec2(1, 1);
    }

    void SyncViews() {
        viewCurve->Update(*modelCurve);

        viewNodes.clear();
        for (const auto& node : modelCurve->Nodes) {
            auto nodeView = std::make_unique<BezierNodeView>(sharedShader.get());
            nodeView->Update(node);
            viewNodes.push_back(std::move(nodeView));
        }
    }

    void OnClick(const glm::vec2& position, ImGuiMouseButton_ button) {
        if (modelCurve.get()->Nodes.empty()) return;
        if (button != ImGuiMouseButton_Left) return;

        glm::vec2 screen = CursorToScreen(position);
        
        float closestDistance = std::numeric_limits<float>::max();
        for (auto& node : modelCurve.get()->Nodes) {
            glm::vec3& center = node.GetPosition();
            glm::vec3& left = node.GetLeftHandle();
            glm::vec3& right = node.GetRightHandle();

            float centerDistance = glm::length(center - glm::vec3(screen, 0));
            float leftDistance = glm::length(left - glm::vec3(screen, 0));
            float rightDistance = glm::length(right - glm::vec3(screen, 0));

            if (centerDistance < closestDistance) {
                selected = &node;
                selectedPartType = HandleType::Center;
                closestDistance = centerDistance;
            }
            if (leftDistance < closestDistance) {
                selected = &node;
                selectedPartType = HandleType::Left;
                closestDistance = leftDistance;
            }
            if (rightDistance < closestDistance) {
                selected = &node;
                selectedPartType = HandleType::Right;
                closestDistance = rightDistance;
            }
        }

        if (closestDistance >= 0.1f) {
            selected = nullptr;
            selectedPartType = HandleType::None;
        }
    }

    void OnDrag(const glm::vec2& totalDelta, const glm::vec2& delta, const glm::vec2& position, ImGuiMouseButton_ button) {
        if (!selected) return;

        const glm::vec3& newPos = glm::vec3(CursorToScreen(position), 0);
        switch(selectedPartType) {
            case HandleType::Center:
                selected->SetPosition(newPos);
            break;
            case HandleType::Left:
                selected->SetLeftHandle(newPos);
            break;
            case HandleType::Right:
                selected->SetRightHandle(newPos);
            break;
            
            default: break;
        }
        
        SyncViews();
    }
};