#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main() {
    float d = length(gl_PointCoord - vec2(0.5));
    if (d > 0.5) discard;

    // Hard core with soft edge
    float alpha = 1.0 - smoothstep(0.35, 0.5, d);
    FragColor = vec4(vColor, alpha);
}