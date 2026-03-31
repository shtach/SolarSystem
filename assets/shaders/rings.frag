#version 330 core

in vec3  vColor;
in vec3  vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;

out vec4 FragColor;

void main() {
    if (vHasAtmosphere < 0.5) discard;  // was > 0.5 — inverted

    vec2  coord  = gl_PointCoord - vec2(0.5);
    float dist   = length(coord);
    float planetRadius = 0.5 / max(vAtmosphereSize, 0.0001);

    // Saturn rings — size 40.0 in your simulation
    if (vSize > 38.0 && vSize < 42.0) {
        float innerRing = planetRadius * 1.3;
        float outerRing = planetRadius * 2.2;

        if (dist > innerRing && dist < outerRing) {
            float ringPattern = mod(dist * 30.0, 1.0);
            if (ringPattern > 0.25) {
                float fade  = 1.0 - smoothstep(innerRing, outerRing, dist);
                float alpha = 0.7 * fade * smoothstep(innerRing, innerRing + 0.02, dist);
                FragColor = vec4(0.92, 0.84, 0.65, alpha);
                return;
            }
        }
    }

    // Atmosphere for all other bodies
    if (dist < planetRadius || dist > 0.5) discard;

    float a1    = 1.0 - smoothstep(planetRadius, 0.5 * 0.7, dist);
    float a2    = 1.0 - smoothstep(0.5 * 0.7, 0.5, dist);
    float alpha = (a1 * 0.8 + a2 * 0.2) * 0.4;

    if (vSize > 35.0) alpha *= 2.0;  // Sun

    FragColor = vec4(vAtmosphereColor, alpha);
}