#pragma once

#include <glm/glm.hpp>
#include "gl_engine/transform.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"

class Object {
public:
    Transform* transform;
    Mesh* mesh;
    Material* material;

    Object(Transform* transform, Mesh* mesh, Material* material) : transform(transform), mesh(mesh), material(material) { }

    virtual void Draw() const {
        //TODO transform.EmitMatrix(); ez beállíthatná a shadert
        material->Bind();
        mesh->Bind();
        mesh->Draw();
    }
};