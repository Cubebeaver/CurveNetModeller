#version 330 core
out vec4 FragColor;

in vec3 viewDir;
uniform sampler2D skyboxTexture;
uniform float brightness;

const vec2 invAtan = vec2(0.1591, 0.3183); // 1/(2*PI), 1/PI

vec2 SampleEquirectangular(vec3 v) {
    vec3 d = normalize(v);
    // Szférikus koordináták kiszámítása
    vec2 uv = vec2(atan(d.z, d.x), asin(d.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleEquirectangular(viewDir);
    vec3 color = texture(skyboxTexture, uv).rgb;

    FragColor = vec4(color, 1.0) * brightness;
}