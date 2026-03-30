#version 330 core
in  vec3 vColor;
out vec4 FragColor;

void main() {
    float d = length(gl_PointCoord - vec2(0.5));
    if (d > 0.6) discard;
    float alpha = pow(1.0 - smoothstep(0.0, 0.6, d), 0.6);
    FragColor = vec4(vColor * 2.0, alpha);
}