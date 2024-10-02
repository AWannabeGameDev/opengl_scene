#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_color;
//layout(location = 2) in mat4 v_model;

uniform mat4 u_view;
uniform mat4 u_proj;

out vsOut
{
    vec3 color;
} outp;

void main()
{
    gl_Position = u_proj * u_view * /*v_model **/ vec4(v_pos, 1.0f);
    outp.color = v_color;
}