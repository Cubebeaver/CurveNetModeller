#pragma once

#include <iostream>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/mesh.h"
#include "engine/material.h"



template<typename T>
std::ostream& operator<<(std::ostream& os, const glm::vec<2, T>& vec) {
    os << "{ " << vec.x << ", " << vec.y << " }";
    return os;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, const glm::vec<3, T>& vec) {
    os << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }";
    return os;
}
template<typename T>
std::ostream& operator<<(std::ostream& os, const glm::vec<4, T>& vec) {
    os << "{ " << vec.x << ", " << vec.y << ", " << vec.z  << ", " << vec.w << " }";
    return os;
}



class Transform
{
private:

public:
    Transform* parent = NULL;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 matTransform;

    void UpdateMatrix();
    float* GetMatricPtr() { return glm::value_ptr(matTransform); }

    Transform();
    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_);
    Transform(glm::vec3 pos, glm::vec3 rot, float scale_) { Transform(pos, rot, glm::vec3(scale_, scale_, scale_)); }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) { Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ)); }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale_) { Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_)); }
    ~Transform();

    void SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_);
    void SetTransform(glm::vec3 pos, glm::vec3 rot, float scale_) { SetTransform(pos, rot, glm::vec3(scale_, scale_, scale_)); }
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ)); }
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale_) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_)); }

    void SetPosition(glm::vec3 pos);
    void SetRotation(glm::vec3 rot);
    void SetScale   (glm::vec3 scale_);
    void SetPosition(float x, float y, float z) { SetPosition(glm::vec3(x, y, z)); }
    void SetRotation(float x, float y, float z) { SetRotation(glm::vec3(x, y, z)); }
    void SetScale   (float x, float y, float z) { SetScale   (glm::vec3(x, y, z)); }
    void SetScale   (float scale_)              { SetScale   (glm::vec3(scale_, scale_, scale_)); }

    void Translate(glm::vec3 offset);
    void Rotate   (glm::vec3 rotate);
    void Scale    (glm::vec3 scale_);
    void Translate(float x, float y, float z) { Translate(glm::vec3(x, y, z)); }
    void Rotate   (float x, float y, float z) { Rotate   (glm::vec3(x, y, z)); }
    void Scale    (float x, float y, float z) { Scale    (glm::vec3(x, y, z)); }
    void Scale    (float scale_)              { Scale    (glm::vec3(scale_, scale_, scale_)); }
};



class Object
{
private:

public:
    Transform* transform;
    unsigned int unilocTransform;
    unsigned int unilocView;
    unsigned int unilocProjection;
    Mesh* mesh;
    Material* material;

    Object();
    //Object(bool genTransform = true, bool genMesh = true, bool genMaterial = true, Shader* shader = NULL, const char* transformName = NULL, const char* viewName = NULL, const char* projectionName = NULL);
    Object(Mesh* mesh_, Material* material_, const char* transformName = "Transform", const char* viewName = "View", const char* projectionName = "Projection");
    ~Object();

    static Object* SuperGenerate(const char* meshPath, const char* vertShaderPath, const char* fragShaderPath, const char* uniformName, const char* texturePath, AlphaMode alphaMode);
    static Object* SuperGenerate(const char* meshPath, Shader* shader, const char* uniformName, const char* texturePath, AlphaMode alphaMode);

    void Draw(unsigned int drawMode = GL_TRIANGLES);
    void GetUniformLocations(const char* transformName, const char* viewName, const char* projectionName);
};