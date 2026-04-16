#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 viewDir;

uniform mat4 invView;       // A View mátrix inverze
uniform mat4 invProjection; // A Projection mátrix inverze

void main() {
    // A quad pozíciója (Z = 1.0, hogy a legtávolabb legyen)
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);

    // Kiszámoljuk a nézeti irányt a világűrben
    vec4 clipPos = vec4(aPos.x, aPos.y, -1.0, 1.0);
    vec4 viewPos = invProjection * clipPos;
    viewPos /= viewPos.w;

    // Az inverz View mátrixszal elforgatjuk a világ koordináta-rendszerébe
    // Csak a rotáció számít (mat3), a transzláció nem
    viewDir = mat3(invView) * viewPos.xyz;
}