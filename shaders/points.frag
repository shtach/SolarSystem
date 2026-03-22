#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main() {
    vec2 c         = gl_PointCoord - vec2(0.5);
    float d        = length(c);

    if (d > 0.5) 
        discard;
    
    float alpha    = 1.0 - smoothstep(0.4, 0.5, d);
    FragColor      = vec4(vColor, alpha);
}