#version 330 core

in vec3 vertPos;

uniform vec4 color;

void main() {
    gl_FragColor = color;
}