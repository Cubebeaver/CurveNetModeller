#pragma once

#include <glad/glad.h>

class Shader
{
private:

public:
    unsigned int program;

    Shader();
    Shader(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath);
    ~Shader();
    void Use() { glUseProgram(program); }
    void SetUniform1i(const char* name, int value)                                              { Use(); glUniform1i       (glGetUniformLocation(program, name), value); }
    void SetUniform1f(const char* name, float value)                                            { Use(); glUniform1f       (glGetUniformLocation(program, name), value); }
    void SetUniform2f(const char* name, float value0, float value1)                             { Use(); glUniform2f       (glGetUniformLocation(program, name), value0, value1); }
    void SetUniform3f(const char* name, float value0, float value1, float value2)               { Use(); glUniform3f       (glGetUniformLocation(program, name), value0, value1, value2); }
    void SetUniform4f(const char* name, float value0, float value1, float value2, float value3) { Use(); glUniform4f       (glGetUniformLocation(program, name), value0, value1, value2, value3); }
    void SetUniformMat4fv(const char* name, bool transpose, float* value)                       { Use(); glUniformMatrix4fv(glGetUniformLocation(program, name), 1, transpose ? GL_TRUE : GL_FALSE, value); }
};