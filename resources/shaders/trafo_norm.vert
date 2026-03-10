#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vPos;
out vec3 vNorm;

void main() {
    vPos = pos;
    vNorm = norm;

    gl_Position = Projection * View * Model * vec4(pos, 1.0f);
}