#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 WorldPos;
out vec3 WorldNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    WorldPos = vec3(Model * vec4(aPos, 1.0));
    // Nem-uniform skálázás esetén normálmátrixot használj: mat3(transpose(inverse(model)))
    WorldNormal = mat3(Model) * aNormal;

    gl_Position = Projection * View * vec4(WorldPos, 1.0);
}