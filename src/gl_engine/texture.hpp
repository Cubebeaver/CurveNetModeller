#pragma once

#include <iostream>

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



enum class AlphaMode {
    Opaque,       // No alpha
    AplhaClip,    // Either opaque or transparent, no inbetween
    AlphaBlend    // Can be translucent
};



class Texture {
public:
    GLuint texture;
    int width, height, numChannel;

    Texture(const Texture&) = delete;

    Texture(const std::string& imagePath, AlphaMode alphaMode = AlphaMode::Opaque, bool genMipMap = true, int wrapMode = GL_REPEAT, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, int minLOD = -1000, int maxLOD = 1000) {
        stbi_set_flip_vertically_on_load(true);
        unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &numChannel, 0);
        if (imageData == nullptr) std::cout << "[-] Failed to load image" << std::endl;

        glGenTextures(1, &texture);
        // A sampler2D uniform erteke egy int ami azt mutatja hogy melyik texture unit-ot hasznalja amikor majd sample-olja.
        // Ezzel a 0. unit-ba toltjuk be a texturat es itt aktivaljuk.
        //glActiveTexture(GL_TEXTURE0); //! Ez kell??
        glBindTexture(GL_TEXTURE_2D, texture);

        // Wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        // Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        // LOD, na a MAX-t  pixel art eseten? pl 32x32 texturanal 5-re 64x64-esnel 6-ra kell allitani
        // mert 2^5=32 es 2^6=64 es arra jo hogy elvileg igy nem lesz bleeding a filtering utan
        // tehat lesz mip map ugyh nem lesz flickering a tavoli texturaknal
        // es a szomdedos sprite se fog at-bleedelni se a masikba
        // A MIN fogalmam sincs mire lehet jo
        // (talan meggatolja az alacsonyabb LODok hasznalatat?? alacsonyabb grafika/optimalizalas eseten?)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, minLOD);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, maxLOD);

        //  void glTexImage2D(
        //      GLenum target,            specifies the texture target; setting this to GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object at the same target (so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_3D will not be affected).
        //      GLint level,              specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we’ll leave it at the base level which is 0.
        //      GLint internalformat,     tells OpenGL in what kind of format we want to store the texture. Our image has only RGB values so we’ll store the texture with RGB values as well.
        //      GLsizei width,            sets the width -
        //      GLsizei height,           and height of the resulting texture. We stored those earlier when loading the image so we’ll use the corresponding variables.
        //      GLint border,             should always be 0 (some legacy stuff).
        //      GLenum format,            specify the format -
        //      GLenum type,              and datatype of the source image. We loaded the image with RGB values and stored them as chars (bytes) so we’ll pass in the corresponding values.
        //      const void *pixels        this is the actual image data.
        //  )
        if (alphaMode == AlphaMode::Opaque)          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  width, height, 0, GL_RGB,  GL_UNSIGNED_BYTE, imageData);
        else if (alphaMode == AlphaMode::AplhaClip)  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        else if (alphaMode == AlphaMode::AlphaBlend) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        else /*  .  .  .  .  .  .  .  .  .  .  .  */ std::cout << "[-] Invalid AlphaMode" << std::endl;
        
        if (genMipMap) glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(imageData);
    }

    ~Texture() {
        glDeleteTextures(1, &texture);
    }

    void BindToUnit(int unit) {
        if (unit >= 16) std::cout << "[!] Warning greater texture unit number than minimum required" << std::endl;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};