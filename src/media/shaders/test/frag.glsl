#version 330 core
out vec4 FragColor;

const float pi = 4.*atan(1.);

in vec2 vPos;

uniform vec3 cameraPos;

void main()
{
    vec2 uv = cameraPos.z*(vPos - cameraPos.xy);
    float s = sin(2.*pi*uv.x)*cos(2.*pi*uv.y);
    FragColor = vec4(s, cos(2.*pi*s), s, 1.0f);
}