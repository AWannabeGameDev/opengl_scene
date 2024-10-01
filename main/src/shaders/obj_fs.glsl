#version 460 core

in vec3 fragColor;

out vec4 finalFragColor;

void main()
{
    finalFragColor = vec4(fragColor, 0.0f);
}