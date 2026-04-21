#pragma once

#include <imgui.h>
#include <iostream>

#include "gl_engine/framebuffer.hpp"
#include "gl_engine/camera.hpp"
using namespace gl_engine;
#include "util/event.hpp"

class Viewport {
public:
    std::unique_ptr<FrameBuffer> viewportBuffer;

    //    Position          MouseButton
    Event<const glm::vec2&, ImGuiMouseButton_> OnClick;
    //    Total delta       Delta             Position          MouseButton
    Event<const glm::vec2&, const glm::vec2&, const glm::vec2&, ImGuiMouseButton_> OnDrag;
    //    Direction (h, v)  Position
    Event<const glm::vec2&, const glm::vec2&> OnScroll;

    Viewport() {
        viewportBuffer = std::make_unique<FrameBuffer>(100, 100);
    }

    ~Viewport() {
        //delete viewportBuffer;
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

                // Camera controls
                ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_None);

                Camera::activeCamera->Rotate(Camera::activeCamera->fieldOfView * -delta.y / viewportBuffer->Height, delta.x / viewportBuffer->Height, 0);
                
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W)) {
                    Camera::activeCamera->Translate(Camera::activeCamera->direction * 0.1f);
                }
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S)) {
                    Camera::activeCamera->Translate(Camera::activeCamera->direction * -0.1f);
                }
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A)) {
                    Camera::activeCamera->Translate(glm::cross(Camera::activeCamera->direction, glm::vec3(0, 1, 0)) * -0.1f);
                }
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D)) {
                    Camera::activeCamera->Translate(glm::cross(Camera::activeCamera->direction, glm::vec3(0, 1, 0)) * 0.1f);
                }
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q)) {
                    Camera::activeCamera->Translate(glm::vec3(0, 1, 0) * -0.1f);
                }
                if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E)) {
                    Camera::activeCamera->Translate(glm::vec3(0, 1, 0) * 0.1f);
                }
            } else {
                ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Arrow);
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