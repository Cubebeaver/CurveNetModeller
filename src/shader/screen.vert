#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nor;
layout (location = 2) in vec4 col;
layout (location = 1) in vec2 tex;

out vec3 vertPos;
out vec3 vertNor;
out vec4 vertCol;
out vec2 vertTex;

void main() {
    vertPos = pos;
    vertNor = nor;
    vertCol = col;
    vertTex = tex;
    
    gl_Position = vec4(pos, 1.0f);
}