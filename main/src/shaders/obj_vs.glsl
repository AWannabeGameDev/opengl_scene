#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in mat4 v_model;
layout(location = 7) in mat3 v_normalMatrix;

layout(std140) uniform cameraMatrices
{
    mat4 u_view;
    mat4 u_proj;
};

layout(std140) uniform dirLightMatrix
{
    mat4 u_dirLightMatrix;
};

out vsOut
{
    vec3 color;
    vec3 normal;
    vec3 fragPos;
    vec3 fragPosForDirLight;
} 
outp;

void main()
{
    outp.fragPos = vec3(v_model * vec4(v_pos, 1.0f));
    gl_Position = u_proj * u_view * vec4(outp.fragPos, 1.0f);
    outp.color = v_color;
    outp.normal = v_normalMatrix * v_normal;
    outp.fragPosForDirLight = vec3(u_dirLightMatrix * vec4(outp.fragPos, 1.0f));
}