#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 3) in mat4 v_model;

layout(std140) uniform dirLightMatrix
{
    mat4 u_dirLightMatrix;
};

void main()
{
    gl_Position = u_dirLightMatrix * v_model * vec4(v_pos, 1.0f);
}