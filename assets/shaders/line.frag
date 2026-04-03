#version 330 core

in float vProgress;   // 0.0 = tail end, 1.0 = near planet
out vec4 FragColor;

uniform vec3  uColor;
uniform float uAlpha;

void main() {
    float fade = pow(vProgress, 2.0);   // quadratic — fades fast at tail
    FragColor = vec4(uColor, uAlpha * fade * 0.5);
}