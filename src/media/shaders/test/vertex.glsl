#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec2 vPos;
out vec2 vOffset;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vPos = vec2(aPos.x, aPos.y);
    vOffset = aOffset;
}