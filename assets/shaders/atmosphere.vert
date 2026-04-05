#version 330 core

layout(location = 0) in vec2  aPos;
layout(location = 1) in float aSize;
layout(location = 2) in vec3  aColor;
layout(location = 3) in float aHasAtmosphere;
layout(location = 4) in float aAtmosphereSize;
layout(location = 5) in vec3  aAtmosphereColor;

out vec3  vColor;
out vec3  vAtmosphereColor;
out float vAtmosphereSize;
out float vHasAtmosphere;
out float vSize;

uniform vec2  uCam;
uniform float uScale;   // was vec2 — this was the GL error

void main() {
    vec2 p       = (aPos - uCam) * uScale;
    gl_Position  = vec4(p, 0.0, 1.0);
    float zoomFactor = clamp(uScale / 2e-13, 0.15, 1.0);
    gl_PointSize = aSize * aAtmosphereSize * zoomFactor;

    vColor           = aColor;
    vAtmosphereColor = aAtmosphereColor;
    vAtmosphereSize  = aAtmosphereSize;
    vHasAtmosphere   = aHasAtmosphere;
    vSize            = aSize;
}