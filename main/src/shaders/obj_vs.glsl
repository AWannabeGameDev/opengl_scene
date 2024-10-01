#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_color;

out vec3 fragColor;

void main()
{
    gl_Position = vec4(v_pos, 1.0f);
    fragColor = v_color;
}