#include "gl_engine/shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <glad/glad.h>

#include "engine/util.h"



// char* PreProcessShaderSource(const char* sourcePath) {
//     // Read original shader
//     char* shaderSource = readFileToString(sourcePath);

//     while (true) {
//         char* includePosition = strstr(shaderSource, "#include");
//         char includePath[128];
//         int success = sscanf("#include \"%s\"", includePath);
//         printf("Found #include preprocessor with path: \"%s\"", includePath);
//         if (!success) break;

//         char* includeSource = readFileToString(includePath);

//         shaderSource = (char*)realloc(shaderSource, strlen(shaderSource) + strlen(includePosition) - strlen(includePath) - 11 /* strlen("#include \"\"") */);
//     }

//     //return NULL;
// }

Shader::Shader(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath) {
    int success;
    char infolog[1024];
    
    unsigned int vertShader = -1;
    unsigned int fragShader = -1;
    
    // Vertex shader
    if (vertexShaderSourcePath != NULL) {
        const char* vertShaderSource = readFileToString(vertexShaderSourcePath);
        if (vertShaderSource == NULL) printf("[-] Failed to read vertShaderSource\n");

        vertShader = glCreateShader(GL_VERTEX_SHADER);
        // Attach the shader source code to the shader object and compile the shader
        glShaderSource(vertShader, 1, &vertShaderSource, NULL); // csak 1 shadert akarunk compileolni, a NULL pedig a source file hossza
        glCompileShader(vertShader);
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertShader, 1024, NULL, infolog);
            printf("[-] Failed to compile vertex shader; log: \n%s\n", infolog);
        }

        free((void*)vertShaderSource);
    } else { printf("[i] Shader has no vertex shader specified\n"); }


    // Fragment shader
    if (fragmentShaderSourcePath != NULL) {
        const char* fragShaderSource = readFileToString(fragmentShaderSourcePath);
        if (fragShaderSource == NULL) printf("[-] Failed to read fragShaderSource\n");

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        // Attach the shader source code to the shader object and compile the shader
        glShaderSource(fragShader, 1, &fragShaderSource, NULL); // csak 1 shadert akarunk compileolni, a NULL pedig a source file hossza
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragShader, 1024, NULL, infolog);
            printf("[-] Failed to compile fragment shader; log: \n%s\n", infolog);
        }

        free((void*)fragShaderSource);
    } else { printf("[i] Shader has no fragment shader specified\n"); }

    // Shader program
    program = glCreateProgram();
    // Hozzaadjuk a vertex es a fragment shader-t is a programhoz
    if (vertShader != -1) glAttachShader(program, vertShader);
    if (fragShader != -1) glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 1024, NULL, infolog);
        printf("[-] Failed to link shaders to program; log: \n%s\n", infolog);
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader() {
    glDeleteProgram(program);
}