#version 460 core

in vsOut
{
    vec3 color;
} inp;

out vec4 fragColor;

void main()
{
    fragColor = vec4(inp.color, 0.0f);
}