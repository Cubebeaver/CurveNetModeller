#pragma once

#include <imgui.h>
#include <iostream>

#include "gl_engine/framebuffer.hpp"
#include "util/event.hpp"

class Viewport {
public:
    FrameBuffer* viewportBuffer;

    //    Position          MouseButton
    Event<const glm::vec2&, ImGuiMouseButton_> OnClick;
    //    Total delta       Delta             Position          MouseButton
    Event<const glm::vec2&, const glm::vec2&, const glm::vec2&, ImGuiMouseButton_> OnDrag;
    //    Direction (h, v)  Position
    Event<const glm::vec2&, const glm::vec2&> OnScroll;

    Viewport() {
        viewportBuffer = new FrameBuffer(100, 100);
    }

    ~Viewport() {
        delete viewportBuffer;
    }

    void BindFrameBuffer() {
        viewportBuffer->Bind();
    }
    void UnbindFrameBuffer() {
        viewportBuffer->Unbind();
    }

    void Draw() const {
        ImGui::SetNextWindowSize(ImVec2(720, 720), ImGuiCond_FirstUseEver);
        ImGui::Begin("Viewport");
        
        bool hovered = ImGui::IsWindowHovered();

        if (hovered) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 contentPos = ImGui::GetCursorScreenPos();
            float relativeX = mousePos.x - contentPos.x;
            float relativeY = mousePos.y - contentPos.y;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                OnClick.Invoke(glm::vec2(relativeX, relativeY), ImGuiMouseButton_Left);
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                OnClick.Invoke(glm::vec2(relativeX, relativeY), ImGuiMouseButton_Right);
            }
            

            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImVec2 totalDelta = ImGui::GetMouseDragDelta();

                OnDrag.Invoke(glm::vec2(totalDelta.x, totalDelta.y), glm::vec2(delta.x, delta.y), glm::vec2(relativeX, relativeY), ImGuiMouseButton_Left);
            }
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImVec2 totalDelta = ImGui::GetMouseDragDelta();

                OnDrag.Invoke(glm::vec2(totalDelta.x, totalDelta.y), glm::vec2(delta.x, delta.y), glm::vec2(relativeX, relativeY), ImGuiMouseButton_Right);

                Camera::activeCamera->Translate(Camera::activeCamera->fieldOfView * -delta.x / viewportBuffer->Height, Camera::activeCamera->fieldOfView * delta.y / viewportBuffer->Height, 0);
            }

            
            float wheel = ImGui::GetIO().MouseWheel;
            float wheelH = ImGui::GetIO().MouseWheelH;
            
            if (wheel != 0 || wheelH != 0) {
                OnScroll.Invoke(glm::vec2(wheelH, wheel), glm::vec2(relativeX, relativeY));

                Camera::activeCamera->fieldOfView *= 1 + 0.1f * -wheel;
                Camera::activeCamera->UpdateProjection();
            }
        }

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        viewportBuffer->UpdateSize(viewportPanelSize.x, viewportPanelSize.y);
        ImGui::Image((ImTextureID)viewportBuffer->colorBuffer, ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2(0, 1), ImVec2(1, 0));
        
        ImGui::End();
    }
};