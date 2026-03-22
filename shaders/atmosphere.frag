#version 330 core
in vec3  vColor;
in vec3  vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;
out vec4 FragColor;

void main() {
    if (vHasAtmosphere < 0.5) discard;
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    float planetRadius = 0.5 / vAtmosphereSize;
    if (dist < planetRadius || dist > 0.5) discard;
    float a1 = 1.0 - smoothstep(planetRadius, 0.5 * 0.7, dist);
    float a2 = 1.0 - smoothstep(0.5 * 0.7, 0.5, dist);
    float alpha = (a1 * 0.8 + a2 * 0.2) * 0.4;
    if (vSize > 35.0) alpha *= 2.0;
    FragColor = vec4(vAtmosphereColor, alpha);
}