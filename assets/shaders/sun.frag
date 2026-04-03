#version 330 core

in vec3 vColor;
out vec4 FragColor;

uniform float uTime;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    float angle = atan(coord.y, coord.x);

    if (dist > 0.5) discard;

    float pulse = sin(uTime * 1.5) * 0.04 + 0.96;
    float d     = dist / pulse;

    // color gradient
    vec3 core  = vec3(1.0,  1.0,  0.95);
    vec3 mid   = vec3(1.0,  0.75, 0.1);
    vec3 rim   = vec3(0.85, 0.2,  0.0);
    float t    = smoothstep(0.0, 0.5, d);
    vec3 color = mix(core, mid, t);
    color      = mix(color, rim, smoothstep(0.4, 1.0, d));

    // corona spikes — sine waves in angle space
    float spike = sin(angle * 8.0 + uTime * 2.0) * 0.5 + 0.5;
    spike       = pow(spike, 6.0);
    float corona = spike * (1.0 - smoothstep(0.35, 0.5, dist)) * 0.6;

    // alpha — solid core fading at rim + corona
    float alpha = (1.0 - smoothstep(0.3, 0.5, d)) + corona;
    alpha       = clamp(alpha, 0.0, 1.0);

    FragColor = vec4(color, alpha);
}