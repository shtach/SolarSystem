#version 330 core

in vec3  vColor;
in vec2  vWorldPos;
in float vSize;

out vec4 FragColor;

uniform vec2  uSunPos;
uniform float uTime;

// --- Value noise ---
float hash(vec2 p) {
    p = fract(p * vec2(127.1, 311.7));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(hash(i),                  hash(i + vec2(1.0, 0.0)), f.x),
        mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x),
        f.y
    );
}

void main() {
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    if (dist > 0.5) discard;

    float alpha = 1.0 - smoothstep(0.38, 0.50, dist);

    // --- Day / Night terminator ---
    vec2  toSun = normalize(uSunPos - vWorldPos);
    vec2  N     = normalize(coord + vec2(0.0001));
    float light = dot(N, toSun);
    float shade = clamp(light * 0.5 + 0.65, 0.12, 1.0);

    // --- Per-planet type visual ---
    vec2  seed    = fract(vWorldPos * 1e-11) * 100.0;
    vec3  color   = vColor;

    // Gas giants: Jupiter(58), Saturn(50), Uranus(35), Neptune(35)
    if (vSize > 30.0) {
        // Animated horizontal cloud bands
        float drift = uTime * 0.04;
        float y     = coord.y / 0.5;  // -1..1

        float b1 = sin(y * 10.0 * 3.14159 + drift)        * 0.5 + 0.5;
        float b2 = sin(y * 17.0 * 3.14159 - drift * 1.3)  * 0.5 + 0.5;
        float band = mix(b1, b2, 0.4);

        // Darken the band valleys, brighten the peaks
        color = mix(vColor * 0.65, vColor * 1.15, band);

        // Storm spot for Jupiter (size ~58) — rough oval near equator
        if (vSize > 54.0) {
            vec2 stormCenter = vec2(0.1, 0.08);
            float stormDist  = length((coord - stormCenter) * vec2(1.0, 1.8));
            float storm      = 1.0 - smoothstep(0.04, 0.09, stormDist);
            color = mix(color, vec3(0.75, 0.28, 0.18), storm * 0.7);
        }

        // Surface noise overlay (subtle)
        float n = noise(coord * 5.0 + seed) * 0.12 + 0.88;
        color *= n;

    } else {
        // Rocky planets: surface noise + terrain variation
        float n1 = noise(coord * 6.0 + seed)       * 0.5 + 0.5;
        float n2 = noise(coord * 12.0 + seed * 2.0) * 0.5 + 0.5;
        float terrain = n1 * 0.7 + n2 * 0.3;

        // Slight color variation (darker lowlands, lighter highlands)
        color = mix(vColor * 0.78, vColor * 1.08, terrain);
    }

    FragColor = vec4(color * shade, alpha);
}
