#version 330 core

in vec3  vColor;
in float vTexIndex;

out vec4 FragColor;

uniform sampler2DArray uTextures;
uniform float uTime;

void main() {
    vec2  coord = gl_PointCoord - vec2(0.5);
    float dist  = length(coord);
    if (dist > 0.5) discard;

    // Texture sample
    vec2 uv = vec2(gl_PointCoord.x, 1.0 - gl_PointCoord.y);
    uv = uv * 2.0 - 1.0;
    if (length(uv) > 1.0) discard;
    uv = uv * 0.5 + 0.5;

    vec3 texColor = texture(uTextures, vec3(uv, vTexIndex)).rgb;

    // Warm tint — shift toward yellow-orange regardless of source texture
    vec3 sunTint = vec3(1.0, 0.82, 0.2);
    vec3 color   = texColor * sunTint * 1.4;

    // Subtle slow pulse on brightness only
    float pulse = sin(uTime * 0.8) * 0.04 + 0.96;
    color *= pulse;

    // Soft radial falloff toward limb
    float limb  = 1.0 - smoothstep(0.25, 0.5, dist);
    float alpha = limb;

    // Bright core
    color = mix(color * 0.85, color * 1.15, 1.0 - smoothstep(0.0, 0.3, dist));

    FragColor = vec4(clamp(color, 0.0, 1.5), clamp(alpha, 0.0, 1.0));
}
