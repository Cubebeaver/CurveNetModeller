#pragma once

#include <glad/glad.h>

enum class AlphaMode {
    // No alpha
    Opaque,
    // Either opaque or transparent, no inbetween
    AplhaClip,
    // Can be translucent
    AlphaBlend
};

class Texture
{
private:
    
public:
    unsigned int texture;
    int width, height, numChannel;

    Texture() = delete;
    Texture(const char* source, AlphaMode alphaMode = AlphaMode::Opaque, bool genMipMap = true, int wrapMode = GL_REPEAT, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, int minLOD = -1000, int maxLOD = 1000);
    ~Texture();
    void Bind() { glBindTexture(GL_TEXTURE_2D, texture); }
    void BindToUnit(int unit);
};