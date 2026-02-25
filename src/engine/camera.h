#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/mesh.h"
#include "engine/material.h"

#define M_PI 3.14159265358979323846


//! Az egesz kamera pozicionalasat ujra kellene csinalni mert ez tragedia
//! Minden ami baj lehet az van ezzel a szarral >:(
//! Ezt fejdalmas lesz ujrairni
class Camera
{
private:

public:
    static Camera* activeCamera;

    static int screenWidth;
    static int screenHeight;
    static float aspect;
    
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
    Camera() { Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), M_PI / 2, 0.1f, 100.0f); }
    Camera(glm::vec3 pos, glm::vec3 rot, float fov, float zNear, float zFar);
    Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar);
    Camera(float fov, float zNear, float zFar);
    Camera(glm::vec3 pos, glm::vec3 rot);
    Camera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    ~Camera();

    void UpdateView();
    void UpdateProjection();

    void SetPosition(glm::vec3 pos);
    void SetPosition(float posX, float posY, float posZ) { SetPosition(glm::vec3(posX, posY, posZ)); }
    void SetRotation(glm::vec3 rot);
    void SetRotation(float rotX, float rotY, float rotZ) { SetRotation(glm::vec3(rotX, rotY, rotZ)); }
    void SetTransform(glm::vec3 pos, glm::vec3 rot);
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ)); }

    void Translate(glm::vec3 offset);
    void Translate(float posX, float posY, float posZ) { Translate(glm::vec3(posX, posY, posZ)); }
    void Rotate(glm::vec3 rot);
    void Rotate(float rotX, float rotY, float rotZ) { Rotate(glm::vec3(rotX, rotY, rotZ)); }
    void LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0));
    void LookAt(float posX, float posY, float posZ, float targX, float targY, float targZ, float upX = 0, float upY = 1, float upZ = 0) { LookAt(glm::vec3(posX, posY, posZ), glm::vec3(targX, targY, targZ), glm::vec3(upX, upY, upZ)); }
    void LookAt(glm::vec3 target) { LookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f)); }
    void LookAt(float targX, float targY, float targZ) { LookAt(position, glm::vec3(targX, targY, targZ), glm::vec3(0.0f, 1.0f, 0.0f)); }

    void SetFieldOfView(float fov);
    void SetNearClippingPlane(float zNear);
    void SetFarClippingPlane(float zFar);
    void SetFrustum(float fov, float zNear, float zFar);

    void SetCamera(glm::vec3 pos, glm::vec3 rot, float fov, float zNear, float zFar) { SetTransform(pos, rot); SetFrustum(fov, zNear, zFar); }
    void SetCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov, float zNear, float zFar) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ)); SetFrustum(fov, zNear, zFar); }

    static void SetActiveCamera(Camera* camera);
    static float* GetActiveViewMatrix() { return glm::value_ptr(activeCamera->matView); }
    static float* GetActiveProjectionMatrix() { return glm::value_ptr(activeCamera->matProjection); }
};