#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <vector>

#include "util/util.hpp"

class Shader {
public:
    GLuint program;

    Shader(const Shader&) = delete;

    Shader(const std::filesystem::path& vertexShaderSourcePath, const std::filesystem::path& fragmentShaderSourcePath) {
        std::string vertexSource = ReadAllText(vertexShaderSourcePath);
        std::string fragmentSource = ReadAllText(fragmentShaderSourcePath);

        if (vertexSource.empty() || fragmentSource.empty()) {
            std::cout << "[!] VertexSource or FragmentSource is empty" << std::endl;
            return;
        }

        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER, "VERTEX");
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER, "FRAGMENT");

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Use() const { glUseProgram(program); }

private:
    GLuint compileShader(const std::string& source, GLenum type, std::string typeName) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        checkCompileErrors(shader, typeName);
        return shader;
    }

    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<char> infoLog(logLength);
                glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
                std::cerr << "[-] Shader Compilation Error (" << type << "):\n" << infoLog.data() << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<char> infoLog(logLength);
                glGetProgramInfoLog(shader, logLength, nullptr, infoLog.data());
                std::cerr << "[-] Program Linking Error:\n" << infoLog.data() << std::endl;
            }
        }
    }
};