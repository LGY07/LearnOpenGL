#version 460 core

in vec3 vColOff;
out vec4 FragColor;

uniform float uTime;

void main()
{
    vec3 color = vec3(
    sin(uTime + vColOff.x),
    sin(uTime + vColOff.y + 2.0),
    sin(uTime + vColOff.z + 4.0)
    ) * 0.5 + 0.5;

    FragColor = vec4(color, 1.0);
}