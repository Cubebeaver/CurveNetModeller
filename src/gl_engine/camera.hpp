#pragma once

#include <algorithm>
#include <glad/glad.h>
#include <iostream>
#include <numbers>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"


namespace gl_engine {

//TODO itt a kamerában is áttérni Ray-re
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(glm::vec3 origin, glm::vec3 direction) {
        this->origin = origin;
        this->direction = direction;
    }
};

//! Az egesz kamera pozicionalasat ujra kellene csinalni mert ez tragedia
//! Minden ami baj lehet az van ezzel a szarral >:(
//! Ezt fejdalmas lesz ujrairni
class Camera
{
protected:

public:
    inline static Camera* activeCamera;

    inline static int screenWidth;
    inline static int screenHeight;
    inline static float aspect;

    // x, y, z - pitch, yaw, roll - (up-down, left-right, tilt)
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 direction;

    float fieldOfView;
    float nearClippingPlane;
    float farClippingPlane;

    glm::mat4 matView;
    glm::mat4 matProjection;


    //! ez igy nem jo sztem
    Camera(const glm::vec3& pos, const glm::vec3& rot, float fov, float zNear, float zFar)
        : position(pos), rotation(rot), fieldOfView(fov), nearClippingPlane(zNear), farClippingPlane(zFar) { }

    void Init() {
        if (Camera::activeCamera == nullptr) {
            Camera::SetActiveCamera(this);
        }

        UpdateView();
        UpdateProjection();
    }

    Camera() : Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), std::numbers::pi_v<float> / 2, 0.1f, 100.0f) { }
    Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar)
        : Camera(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), fov, zNear, zFar) { }
    Camera(float fov, float zNear, float zFar) : Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), fov, zNear, zFar) { }
    Camera(glm::vec3 pos, glm::vec3 rot) : Camera(pos, rot, std::numbers::pi_v<float> / 2, 0.1f, 100.0f) { }
    Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) : Camera(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), std::numbers::pi_v<float> / 2, 0.1f, 100.0f) { }
    ~Camera() {
        if (Camera::activeCamera == this) SetActiveCamera(nullptr);
    }

    void UpdateFrameSize(int x, int y) {
        screenWidth = x;
        screenHeight = y;
        aspect = static_cast<float>(x) / static_cast<float>(y);

        UpdateProjection();
    }

    void UpdateView() {
        const glm::vec3 up(0.0f, 1.0f, 0.0f);

        //rotation.x = std::clamp<float>(rotation.x, std::numbers::pi_v<float> / 1.999, 3 * std::numbers::pi_v<float> / 2.001);

        direction = glm::vec3(
            sin(rotation.y) * cos(rotation.x),
            -sin(rotation.x),
            -cos(rotation.y) * cos(rotation.x)
        );

        matView = glm::lookAt(position, position + direction, up);
    }
    virtual void UpdateProjection() = 0;

    void SetPosition(glm::vec3 pos) {
        position = pos;

        UpdateView();
    }
    void SetPosition(float posX, float posY, float posZ) { SetPosition(glm::vec3(posX, posY, posZ)); }
    void SetRotation(glm::vec3 rot) {
        rotation = rot;

        UpdateView();
    }
    void SetRotation(float rotX, float rotY, float rotZ) { SetRotation(glm::vec3(rotX, rotY, rotZ)); }
    void SetTransform(glm::vec3 pos, glm::vec3 rot) {
        position = pos;
        rotation = rot;

        UpdateView();
    }
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ)); }

    void Translate(glm::vec3 offset) {
        position += offset;

        UpdateView();
    }
    void Translate(float posX, float posY, float posZ) { Translate(glm::vec3(posX, posY, posZ)); }
    void Rotate(glm::vec3 rot) {
        rotation += rot;

        UpdateView();
    }
    void Rotate(float rotX, float rotY, float rotZ) { Rotate(glm::vec3(rotX, rotY, rotZ)); }
    void LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0)) {
        std::cout << "[-] void Camera::LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up) not implemented" << std::endl;

        position = pos;
        direction = glm::normalize(target - pos);
        rotation = glm::vec3(
            atan2f( sqrtf(direction.x*direction.x + direction.z*direction.z), direction.y ),
            atan2f( direction.x, direction.z ),
            std::numbers::pi_v<float>
        );

        matView = glm::lookAt(pos, target, up);
    }
    void LookAt(float posX, float posY, float posZ, float targX, float targY, float targZ, float upX = 0, float upY = 1, float upZ = 0) { LookAt(glm::vec3(posX, posY, posZ), glm::vec3(targX, targY, targZ), glm::vec3(upX, upY, upZ)); }
    void LookAt(glm::vec3 target) { LookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f)); }
    void LookAt(float targX, float targY, float targZ) { LookAt(position, glm::vec3(targX, targY, targZ), glm::vec3(0.0f, 1.0f, 0.0f)); }

    void SetFieldOfView(float fov) {
        fieldOfView = fov;

        UpdateProjection();
    }
    void SetNearClippingPlane(float zNear) {
        nearClippingPlane = zNear;

        UpdateProjection();
    }
    void SetFarClippingPlane(float zFar) {
        farClippingPlane = zFar;

        UpdateProjection();
    }
    void SetFrustum(float fov, float zNear, float zFar) {
        fieldOfView = fov;
        nearClippingPlane = zNear;
        farClippingPlane = zFar;

        UpdateProjection();
    }


    virtual glm::vec3 GetRayDirectionFromScreen(float mouseX, float mouseY) const = 0;

    glm::vec3 GetRayDirectionFromScreen(const glm::vec2& mousePos) const {
        return GetRayDirectionFromScreen(mousePos.x, mousePos.y);
    }

    float DistanceToRay(const glm::vec3& pointPosition, const glm::vec3& rayDirection) const {
        glm::vec3 camToNode = pointPosition - position;
        float distanceToRay = glm::length(glm::cross(camToNode, rayDirection));
        return distanceToRay;
    }

    float DistanceToRay(const glm::vec3& pointPosition, float mouseX, float mouseY) const {
        return DistanceToRay(pointPosition, GetRayDirectionFromScreen(mouseX, mouseY));
    }

    float DistanceToRay(const glm::vec3& pointPosition, const glm::vec2& mousePos) const {
        return DistanceToRay(pointPosition, GetRayDirectionFromScreen(mousePos));
    }


    void SetCamera(glm::vec3 pos, glm::vec3 rot, float fov, float zNear, float zFar) { SetTransform(pos, rot); SetFrustum(fov, zNear, zFar); }
    void SetCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ)); SetFrustum(fov, zNear, zFar); }

    static void SetActiveCamera(Camera* cam) {
        activeCamera = cam;
        // Ha az utolso hasznalat ota megvaltozott volna az aspect
        if (cam) {
            activeCamera->UpdateProjection();
        }
    }
};



class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const glm::vec3& pos, const glm::vec3& rot, float fov, float zNear, float zFar) : Camera(pos, rot, fov, zNear, zFar) { Init(); }
    PerspectiveCamera() : Camera() { Init(); }
    PerspectiveCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar) : Camera(posX, posY, posZ, rotX, rotY, rotZ, fov, zNear, zFar) { Init(); }
    PerspectiveCamera(float fov, float zNear, float zFar) : Camera(fov, zNear, zFar) { Init(); }
    PerspectiveCamera(glm::vec3 pos, glm::vec3 rot) : Camera(pos, rot) { Init(); }
    PerspectiveCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) : Camera(posX, posY, posZ, rotX, rotY, rotZ) { Init(); }


    virtual void UpdateProjection() override {
        matProjection = glm::perspective(
            fieldOfView,
            Camera::aspect,
            nearClippingPlane,
            farClippingPlane
        );
    }

    virtual glm::vec3 GetRayDirectionFromScreen(float mouseX, float mouseY) const override {
        float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;

        // NDC -> Clip Space
        glm::vec4 ray_clip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

        // Clip Space -> Eye Space
        glm::vec4 ray_eye = glm::inverse(matProjection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        // Eye Space -> World Space
        glm::vec3 ray_world = glm::vec3(glm::inverse(matView) * ray_eye);

        return glm::normalize(ray_world);
    }
};

class OrthoCamera : public Camera {
public:
    OrthoCamera(const glm::vec3& pos, const glm::vec3& rot, float fov, float zNear, float zFar) : Camera(pos, rot, fov, zNear, zFar) { Init(); }
    OrthoCamera() : Camera() { Init(); }
    OrthoCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar) : Camera(posX, posY, posZ, rotX, rotY, rotZ, fov, zNear, zFar) { Init(); }
    OrthoCamera(float fov, float zNear, float zFar) : Camera(fov, zNear, zFar) { Init(); }
    OrthoCamera(glm::vec3 pos, glm::vec3 rot) : Camera(pos, rot) { Init(); }
    OrthoCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) : Camera(posX, posY, posZ, rotX, rotY, rotZ) { Init(); }

    virtual void UpdateProjection() override {
        matProjection = glm::ortho(
            -fieldOfView * aspect / 2,
            fieldOfView * aspect / 2,
            -fieldOfView / 2,
            fieldOfView / 2,
            nearClippingPlane,
            farClippingPlane
        );
    }

    virtual glm::vec3 GetRayDirectionFromScreen(float mouseX, float mouseY) const override {
        float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;

        // NDC -> Clip Space
        glm::vec4 ray_clip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

        // Clip Space -> Eye Space
        glm::vec4 ray_eye = glm::inverse(matProjection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        // Eye Space -> World Space
        glm::vec3 ray_world = glm::vec3(glm::inverse(matView) * ray_eye);

        return glm::normalize(ray_world);
    }
};

} // namespace gl_engine
