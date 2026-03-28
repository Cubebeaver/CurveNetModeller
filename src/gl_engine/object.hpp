#pragma once

#include <glm/glm.hpp>
#include "gl_engine/transform.hpp"
#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"

class Object {
public:
    std::shared_ptr<Transform> transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    Object(std::shared_ptr<Transform> transform, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
        : transform(transform), mesh(mesh), material(material) { }

    virtual void Draw() const {
        //TODO transform.EmitMatrix(); ez beállíthatná a shadert
        material->Bind();
        mesh->Bind();
        mesh->Draw();
    }
};