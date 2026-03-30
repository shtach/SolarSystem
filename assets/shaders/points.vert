#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in vec3 aColor;

out vec3 ourColor;

uniform vec2 uCam;
uniform vec2 uScale;

void main() {
    gl_Position    = vec4((aPos - uCam) * uScale, 0.0, 1.0);
    gl_PointSize   = aSize;
    ourColor       = aColor;
}