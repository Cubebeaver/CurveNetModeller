#include "engine/material.h"

#include <vector>
#include <stdio.h>

#include "engine/shader.h"
#include "engine/texture.h"



Material::Material(Shader* shader_)
{
    shader = shader_;
}

Material::~Material()
{
    delete shader;
    for (auto tex : textures) delete tex;

#ifdef DEBUG
    printf("[?] Material::~Material() implemented???\n");
#endif
}

void Material::AddTexture(const char* path, AlphaMode mode) {
    Texture* tex = new Texture(path, mode);
    textures.push_back(tex);
}
void Material::AddTexture(Texture* texture) {
    textures.push_back(texture);
}
void Material::Use() {
    shader->Use();
    
    for (int i = 0; i < textures.size(); i++) {
        textures[i]->BindToUnit(i);
    }
}
