#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main() {
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    if (dist > 0.5) discard;

    // Exponential falloff — tight, realistic halo
    float glow = exp(-dist * 7.0) * 0.75;

    // Shift toward white-hot at center
    vec3 color = mix(vColor, vec3(1.0, 0.95, 0.8), smoothstep(0.2, 0.0, dist) * 0.5);

    FragColor = vec4(color, glow);
}
