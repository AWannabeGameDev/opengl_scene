#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 3) in mat4 v_model;
layout(location = 7) in mat3 v_normalMatrix;

out vsOut
{
    vec3 vertPos;
    vec3 vertNormal;
}
outp;

void main()
{
    gl_Position = vec4(0.0f);
    outp.vertPos = vec3(v_model * vec4(v_pos, 1.0f));
    outp.vertNormal = v_normalMatrix * v_normal;
}