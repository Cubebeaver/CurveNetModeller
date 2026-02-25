#version 330

in vec3 vertPos;
in vec3 vertNor;
in vec4 vertCol;
in vec2 vertTex;

void main() {
    gl_FragColor = vertCol;
}