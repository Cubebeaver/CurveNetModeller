#include <glad/glad.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <ctime>

std::string GetCurrentTimeString() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    std::stringstream ss;
    ss << std::put_time(now, "%Y-%m-%d_%H-%M-%S");

    return ss.str();
}

void CaptureScreenshot(GLuint fboID, int width, int height) {
    std::string savename = "output/screenshots/" + GetCurrentTimeString() + "_screenshot.png";
    std::vector<unsigned char> pixels(width * height * 3);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    stbi_flip_vertically_on_write(true);
    if (stbi_write_png(savename.c_str(), width, height, 3, pixels.data(), width * 3)) {
        std::cout << "[+] FBO Screenshot saved: " << savename << std::endl;
    }
}