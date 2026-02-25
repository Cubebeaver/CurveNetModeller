#include "engine/camera.h"

Camera* Camera::activeCamera;
int     Camera::screenWidth;
int     Camera::screenHeight;
float   Camera::aspect;


#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/mesh.h"
#include "engine/material.h"



Camera::Camera(glm::vec3 pos, glm::vec3 rot, float fov, float zNear, float zFar)
{
    if (Camera::activeCamera == NULL)
        Camera::SetActiveCamera(this);

    position          = pos;
    rotation          = rot;
    fieldOfView       = fov;
    nearClippingPlane = zNear;
    farClippingPlane  = zFar;

    UpdateView();
    UpdateProjection();
}

Camera::Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar)
{
    if (Camera::activeCamera == NULL)
        Camera::SetActiveCamera(this);

    position          = glm::vec3(posX, posY, posZ);
    rotation          = glm::vec3(rotX, rotY, rotZ);
    fieldOfView       = fov;
    nearClippingPlane = zNear;
    farClippingPlane  = zFar;

    UpdateView();
    UpdateProjection();
}
Camera::Camera(float fov, float zNear, float zFar)
{
    if (Camera::activeCamera == NULL)
        Camera::SetActiveCamera(this);

    position          = glm::vec3(0, 0, 0);
    rotation          = glm::vec3(0, 0, 0);
    fieldOfView       = fov;
    nearClippingPlane = zNear;
    farClippingPlane  = zFar;

    UpdateView();
    UpdateProjection();
}
Camera::Camera(glm::vec3 pos, glm::vec3 rot)
{
    if (Camera::activeCamera == NULL)
        Camera::SetActiveCamera(this);

    position          = pos;
    rotation          = rot;
    fieldOfView       = M_PI / 2;
    nearClippingPlane = 0.1f;
    farClippingPlane  = 1000.0f;

    UpdateView();
    UpdateProjection();
}
Camera::Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    if (Camera::activeCamera == NULL)
        Camera::SetActiveCamera(this);

    position          = glm::vec3(posX, posY, posZ);
    rotation          = glm::vec3(rotX, rotY, rotZ);
    fieldOfView       = M_PI / 2;
    nearClippingPlane = 0.1f;
    farClippingPlane  = 1000.0f;

    UpdateView();
    UpdateProjection();
}

Camera::~Camera()
{
    if (Camera::activeCamera == this) Camera::activeCamera = nullptr;
}


void Camera::UpdateView() {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    rotation.x = std::clamp<float>(rotation.x, M_PI / 1.999, 3 * M_PI / 2.001);

    direction = glm::vec3(
         sin(rotation.y) * cos(rotation.x),
        -sin(rotation.x),
        -cos(rotation.y) * cos(rotation.x)
    );

    matView = glm::lookAt(position, position + direction, up);
}

void Camera::UpdateProjection() {
    matProjection = glm::perspective(fieldOfView, Camera::aspect, nearClippingPlane, farClippingPlane);
}



void Camera::SetPosition(glm::vec3 pos) {
    position = pos;

    UpdateView();
}

void Camera::SetRotation(glm::vec3 rot) {
    rotation = rot;

    UpdateView();
}

void Camera::SetTransform(glm::vec3 pos, glm::vec3 rot) {
    position = pos;
    rotation = rot;

    UpdateView();
}


void Camera::Translate(glm::vec3 offset) {
    position += offset;

    UpdateView();
}

void Camera::Rotate(glm::vec3 rot) {
    rotation += rot;

    UpdateView();
}

void Camera::LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up) {
    printf("[-] void Camera::LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up) not implemented\n");
    
    position = pos;
    direction = glm::normalize(target - pos);
    rotation = glm::vec3(
        atan2f( sqrtf(direction.x*direction.x + direction.z*direction.z), direction.y ),
        atan2f( direction.x, direction.z ),
        M_PI
    );

    matView = glm::lookAt(pos, target, up);
}



void Camera::SetFieldOfView(float fov) {
    fieldOfView = fov;

    UpdateProjection();
}

void Camera::SetNearClippingPlane(float zNear) {
    nearClippingPlane = zNear;
    
    UpdateProjection();
}

void Camera::SetFarClippingPlane(float zFar) {
    farClippingPlane = zFar;
    
    UpdateProjection();
}

void Camera::SetFrustum(float fov, float zNear, float zFar) {
    fieldOfView = fov;
    nearClippingPlane = zNear;
    farClippingPlane = zFar;

    UpdateProjection();
}



void Camera::SetActiveCamera(Camera* cam) {
    activeCamera = cam;
    // Ha az utolso hasznalat ota megvaltozott volna az aspect
    activeCamera->UpdateProjection(); 
}