#version 330 core

layout(location = 0) in vec2  aPos;
layout(location = 1) in float aProgress;

out float vProgress;

uniform vec2  uCam;
uniform float uScale;

void main() {
    vec2 p = (aPos - uCam) * uScale;
    gl_Position = vec4(p, 0.0, 1.0);
    vProgress = aProgress;
}
