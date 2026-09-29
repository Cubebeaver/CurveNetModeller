#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;

uniform vec3 cameraPos;
uniform float stripeFrequency; // Pl. 20.0 (csíkok sűrűsége)
uniform float stripeSharpness; // Pl. 0.05 (él élessége, 0.5 = sima szinusz, 0.01 = kemény fekete-fehér)

void main()
{
    vec3 N = normalize(WorldNormal);
    vec3 V = normalize(cameraPos - WorldPos);

    // Tükröződési vektor számítása
    vec3 R = reflect(-V, N);

    // Henger-tükrözés modellezése: a csíkok elhelyezkedése a visszavert sugár Y és X tengelye alapján
    // Vízszintes csíkokhoz R.y-t használunk, forgatott csíkokhoz pl. (R.y + R.x * 0.5)
    float stripeCoord = R.y * stripeFrequency;

    // Szinuszhullám generálása a periodikus mintához (-1 és 1 között)
    float pattern = sin(stripeCoord);

    // Éles fekete-fehér átmenet képzése antialiasinggal (smoothstep)
    float stripe = smoothstep(-stripeSharpness, stripeSharpness, pattern);

    // Eredményként fekete-fehér színt adunk vissza
    vec3 color = mix(vec3(0.05), vec3(0.95), stripe);

    FragColor = vec4(color, 1.0);
}