#version 330 core

in float vBrightness;
out vec4 FragColor;

uniform float uTime;

void main() {
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    if (dist > 0.5) discard;

    // Each star twinkles at its own phase (vBrightness used as seed)
    float twinkle = 1.0 + sin(uTime * 2.5 + vBrightness * 137.5) * 0.18;

    // Color: blue-white (dim) → warm white (bright)
    vec3 color = mix(vec3(0.6, 0.75, 1.0), vec3(1.0, 0.97, 0.88), vBrightness);

    float alpha = vBrightness * twinkle * (1.0 - smoothstep(0.2, 0.5, dist));
    FragColor   = vec4(color, clamp(alpha, 0.0, 1.0));
}
