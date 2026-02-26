#include "engine/object.h"

#include <stdio.h>
#include <math.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/mesh.h"
#include "engine/material.h"
#include "engine/camera.h"




/*
Object::Object(bool genTransform, bool genMesh, bool genMaterial, Shader* shader, const char* transformName, const char* viewName, const char* projectionName) {
    if (genTransform) transform = new Transform();
    if (genMesh)      mesh      = new Mesh();
    if (genMaterial)  
        if (shader == NULL)
                      material  = new Material();
        else {
                      material  = new Material(shader);
                      GetUniformLocations();
        }
}
*/

Object::Object(Mesh* mesh_, Material* material_, const char* transformName, const char* viewName, const char* projectionName)
{
    mesh = mesh_; 
    material = material_;

    GetUniformLocations(transformName, viewName, projectionName);

    transform = new Transform();
}

Object::~Object()
{
    delete transform;
    delete mesh;
    delete material;

#ifdef DEBUG
    printf("[?] Object::~Object() implemented???\n");
#endif
}

void Object::Draw(unsigned int drawMode) {
    mesh->Bind();
    material->Use();

    glUniformMatrix4fv(unilocView      , 1, GL_FALSE, Camera::activeCamera->GetActiveViewMatrix()      );
    glUniformMatrix4fv(unilocProjection, 1, GL_FALSE, Camera::activeCamera->GetActiveProjectionMatrix());
    glUniformMatrix4fv(unilocTransform , 1, GL_FALSE,            transform->GetMatricPtr()             );

    glDrawElements(drawMode, mesh->countIndices, GL_UNSIGNED_INT, 0);
}

void Object::GetUniformLocations(const char* transformName, const char* viewName, const char* projectionName) {
    unilocTransform  = glGetUniformLocation(material->shader->program, transformName );
    unilocView       = glGetUniformLocation(material->shader->program, viewName      );
    unilocProjection = glGetUniformLocation(material->shader->program, projectionName);
}



Transform::Transform()
{
    LocalPosition     = glm::vec3(0.0f, 0.0f, 0.0f);
    LocalRotation     = glm::vec3(0.0f, 0.0f, 0.0f);
    LocalScale        = glm::vec3(1.0f, 1.0f, 1.0f);
    LocalMatrix = glm::mat4(1.0f);
}
Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_)
{
    LocalPosition = pos;
    LocalRotation = rot;
    LocalScale    = scale_;

    UpdateMatrix();
}

Transform::~Transform()
{
#ifdef DEBUG
    printf("[-] Transform::~Transform() not implemented\n");
#endif
}


void Transform::UpdateMatrix() {
    LocalMatrix = glm::mat4(1.0f);
    LocalMatrix = glm::translate(LocalMatrix, LocalPosition);
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    LocalMatrix = glm::scale    (LocalMatrix, LocalScale);
}


void Transform::SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_) {
    LocalPosition = pos;
    LocalRotation = rot;
    LocalScale    = scale_;

    UpdateMatrix();
}


void Transform::SetPosition(glm::vec3 pos) {
    LocalPosition = pos;

    UpdateMatrix();
}

void Transform::SetRotation(glm::vec3 rot) {
    LocalRotation = rot;

    UpdateMatrix();
}

void Transform::SetScale(glm::vec3 scale_) {
    LocalScale = scale_;

    UpdateMatrix();
}


void Transform::Translate(glm::vec3 offset) {
    LocalMatrix = glm::translate(LocalMatrix, offset);
    LocalPosition += offset;
}

void Transform::Rotate(glm::vec3 rotate) {
    LocalMatrix = glm::rotate(LocalMatrix, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
    LocalMatrix = glm::rotate(LocalMatrix, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
    LocalMatrix = glm::rotate(LocalMatrix, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));
    LocalRotation += rotate;
}

void Transform::Scale(glm::vec3 scale_) {
    LocalMatrix = glm::scale(LocalMatrix, scale_);
    LocalScale.x *= scale_.x;
    LocalScale.y *= scale_.y;
    LocalScale.z *= scale_.z;
}




Object* Object::SuperGenerate(const char* meshPath, const char* vertShaderPath, const char* fragShaderPath, const char* uniformName, const char* texturePath, AlphaMode alphaMode /*, Mesh** outMesh, Shader** outShader, Material** outMaterial, Texture** outTexture */ ) {
    Mesh* mesh = new Mesh(meshPath);


    Shader* shader = new Shader(vertShaderPath, fragShaderPath);
    if (uniformName)
        shader->SetUniform1i(uniformName, 0);
    
    Material* material = new Material(shader);
    
    if (texturePath) {
        Texture* texture = new Texture(texturePath, alphaMode);
        material->AddTexture(texture);
    }
    

    Object* object = new Object(mesh, material);

    return object;
}

Object* Object::SuperGenerate(const char* meshPath, Shader* shader, const char* uniformName, const char* texturePath, AlphaMode alphaMode) {
    Mesh* mesh = new Mesh(meshPath);
    
    Material* material = new Material(shader);
    
    if (texturePath) {
        Texture* texture = new Texture(texturePath, alphaMode);
        material->AddTexture(texture);
    }
    

    Object* object = new Object(mesh, material);

    return object;
}