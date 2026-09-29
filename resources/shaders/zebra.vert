#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 WorldPos;
out vec3 WorldNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    // Nem-uniform skálázás esetén normálmátrixot használj: mat3(transpose(inverse(model)))
    WorldNormal = mat3(model) * aNormal;

    gl_Position = projection * view * vec4(WorldPos, 1.0);
}