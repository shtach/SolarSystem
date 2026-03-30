#version 330 core
in vec3  vColor;
in vec3  vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;
out vec4 FragColor;

void main() {
    if (vHasAtmosphere > 0.5) 
        discard;

    vec2 coord          = gl_PointCoord - vec2(0.5);
    float dist          = length(coord);
    float palnetRadius  = .5 / vAtmosphereSize;

    // Saturn rings
    if (vSize > 38.0 && vSize < 42.0) {
        if (dist > planetRadius * 1.2 && dist < planetRadius * 2.0) {
            float ring = mod (dist * 20.0, 1.0);

            if (ring > 0.3) {
                float alpha = 0.6 * (1.0 - smoothstep (planetRadius * 1.2, planetRadius * 2.0, dist));
                FragColor = vec4 (0.9, 0.8, 0.6, alpha);
                return;
            }
        }
    }

    if (dist < planetRadius || dist > 0.5)
        discard;

    float a1    = 1.0 - smoothstep (planetRadius, 0.5 * 0.7, dist);
    float a2    = 1.0 - smoothstep (0.5 * 0.7, 0.5, dist);

    float alpha = (a1 * 0.8 + a2 * 0.2) * 0.4;

    if (vSize > 35.0) 
        alpha *= 2.0;

    FragColor = vec4 (vAtmosphereColor, alpha);
}