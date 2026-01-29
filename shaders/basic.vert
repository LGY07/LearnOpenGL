#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColOff;

out vec3 vColOff;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColOff = aColOff;
}