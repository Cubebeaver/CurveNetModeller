#pragma once

#include <iostream>
#include <glad/glad.h>


class FrameBuffer {
public:
    GLuint fbo, colorBuffer;
    int Width, Height;

    FrameBuffer(const FrameBuffer&) = delete;
    
    FrameBuffer(int width, int height)
        : Width(width), Height(height) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "[-] Framebuffer is not complete" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~FrameBuffer() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorBuffer);
    }

    void UpdateSize(int width, int height) {
        if (width == Width && height == Height) return;

        Width = width;
        Height = height;
        
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }

    void Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, Width, Height);
    }

    void Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Clear() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};