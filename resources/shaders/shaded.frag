#version 330 core

uniform vec4 color;

in vec3 vPos;
in vec3 vNorm;

void main() {
    vec3 sun = normalize(vec3(1, 1, 1));
    float light = dot(sun, vNorm);
    float shifted = (light + 1) / 2;
    gl_FragColor = color * shifted;
}