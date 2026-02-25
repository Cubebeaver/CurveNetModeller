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
    position     = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation     = glm::vec3(0.0f, 0.0f, 0.0f);
    scale        = glm::vec3(1.0f, 1.0f, 1.0f);
    matTransform = glm::mat4(1.0f);
}
Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_)
{
    position = pos;
    rotation = rot;
    scale    = scale_;

    UpdateMatrix();
}

Transform::~Transform()
{
#ifdef DEBUG
    printf("[-] Transform::~Transform() not implemented\n");
#endif
}


void Transform::UpdateMatrix() {
    matTransform = glm::mat4(1.0f);
    matTransform = glm::translate(matTransform, position);
    matTransform = glm::rotate   (matTransform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matTransform = glm::rotate   (matTransform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matTransform = glm::rotate   (matTransform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    matTransform = glm::scale    (matTransform, scale);
}


void Transform::SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_) {
    position = pos;
    rotation = rot;
    scale    = scale_;

    UpdateMatrix();
}


void Transform::SetPosition(glm::vec3 pos) {
    position = pos;

    UpdateMatrix();
}

void Transform::SetRotation(glm::vec3 rot) {
    rotation = rot;

    UpdateMatrix();
}

void Transform::SetScale(glm::vec3 scale_) {
    scale = scale_;

    UpdateMatrix();
}


void Transform::Translate(glm::vec3 offset) {
    matTransform = glm::translate(matTransform, offset);
    position += offset;
}

void Transform::Rotate(glm::vec3 rotate) {
    matTransform = glm::rotate(matTransform, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
    matTransform = glm::rotate(matTransform, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
    matTransform = glm::rotate(matTransform, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));
    rotation += rotate;
}

void Transform::Scale(glm::vec3 scale_) {
    matTransform = glm::scale(matTransform, scale_);
    scale.x *= scale_.x;
    scale.y *= scale_.y;
    scale.z *= scale_.z;
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