#version 330 core

in vec3  vColor;
in vec2  vWorldPos;
in float vSize;
in float vTexIndex;

out vec4 FragColor;

uniform sampler2DArray uTextures;
uniform vec2  uSunPos;
uniform float uTime;

// --- Value noise (used for procedural asteroids) ---
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

    vec3 color;

    if (vTexIndex >= 0.0) {
        // --- Textured body: sample the texture array ---
        // gl_PointCoord is (0,0) top-left → flip Y for OpenGL convention
        vec2 uv = vec2(gl_PointCoord.x, 1.0 - gl_PointCoord.y);
        // Crop to circle: remap uv so the disc fills the texture
        uv = uv * 2.0 - 1.0;           // -1..1
        if (length(uv) > 1.0) discard;
        uv = uv * 0.5 + 0.5;           // back to 0..1

        vec4 texSample = texture(uTextures, vec3(uv, vTexIndex));
        color = texSample.rgb;

        // Subtle color tint from body's base color for consistency
        color = mix(color, color * vColor * 2.0, 0.15);

    } else {
        // --- Procedural: asteroids / KBOs ---
        vec2 seed = fract(vWorldPos * 1e-11) * 100.0;
        float n1 = noise(coord * 6.0 + seed)        * 0.5 + 0.5;
        float n2 = noise(coord * 12.0 + seed * 2.0) * 0.5 + 0.5;
        float terrain = n1 * 0.7 + n2 * 0.3;
        color = mix(vColor * 0.78, vColor * 1.08, terrain);
    }

    FragColor = vec4(color * shade, alpha);
}
