#version 330 core

in vec3  vColor;
in vec3  vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;

out vec4 FragColor;

uniform float uTime;

void main() {
    if (vHasAtmosphere < 0.5) discard;

    vec2  coord   = gl_PointCoord - vec2(0.5);
    float dist    = length(coord);
    float planetR = 0.5 / max(vAtmosphereSize, 0.0001);

    // ---- Saturn rings (size ~50) ----
    if (vSize > 48.0 && vSize < 52.0) {
        float innerEdge = planetR * 1.22;
        float outerEdge = 0.488;
        if (dist < innerEdge || dist > outerEdge) discard;

        float t = (dist - innerEdge) / (outerEdge - innerEdge);  // 0..1

        // B ring
        float bRing   = smoothstep(0.0, 0.04, t) * (1.0 - smoothstep(0.42, 0.49, t));
        // Cassini division
        float cassini = 1.0 - smoothstep(0.46, 0.49, t) * (1.0 - smoothstep(0.49, 0.52, t));
        // A ring
        float aRing   = smoothstep(0.51, 0.57, t) * (1.0 - smoothstep(0.87, 0.97, t)) * 0.72;

        float fine  = sin(t * 240.0) * 0.5 + 0.5;
        float mask  = (bRing + aRing) * cassini;
        float alpha = mask * mix(0.55, 0.92, fine) * 0.90;
        if (alpha < 0.02) discard;

        vec3 ringCol = mix(vec3(0.94, 0.87, 0.68), vec3(0.70, 0.60, 0.40), fine * 0.35);
        FragColor = vec4(ringCol, alpha);
        return;
    }

    // ---- Atmosphere ----
    if (dist < planetR || dist > 0.5) discard;

    float t = (dist - planetR) / (0.5 - planetR);  // 0 = planet edge, 1 = outer

    // Bright limb right at the planet's edge
    float rim  = exp(-t * 6.0) * 0.85;

    // Softer outer glow — wider for gas giants
    float haze = (1.0 - smoothstep(0.0, 1.0, t));
    haze = pow(haze, 2.5);

    float alpha = rim * 0.65 + haze * 0.20;

    // Animated shimmer on gas giants
    if (vSize > 30.0) {
        float shimmer = sin(uTime * 0.8 + dist * 40.0) * 0.04 + 1.0;
        alpha *= shimmer * 1.4;
    }

    // Sun corona — very strong
    if (vSize > 52.0) {
        alpha *= 2.8;
    }

    // Rim color brightening (Rayleigh scattering — edge looks lighter)
    vec3 rimColor = mix(vAtmosphereColor, vAtmosphereColor * 1.4 + 0.1, exp(-t * 8.0));

    FragColor = vec4(rimColor, clamp(alpha, 0.0, 1.0));
}
