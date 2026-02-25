#pragma once

#include <vector>

#include "engine/shader.h"
#include "engine/texture.h"



class Material
{
private:

public:
    Shader* shader;

    std::vector<Texture*> textures;

    Material(/* args */);
    Material(Shader* shader);
    ~Material();
    void AddTexture(const char* path, AlphaMode mode = AlphaMode::Opaque);
    void AddTexture(Texture* texture);
    void Use();
};