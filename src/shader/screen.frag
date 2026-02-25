#version 330

in vec3 vertPos;
in vec3 vertNor;
in vec4 vertCol;
in vec2 vertTex;

uniform sampler2D albedo;
uniform vec4 color;

void main() {
    gl_FragColor = texture(albedo, vertTex) * vertCol + color;
}