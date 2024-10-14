#version 460 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_tangent;
layout(location = 4) in mat4 v_model;
layout(location = 8) in mat3 v_normalMatrix;

out vsOut
{
    vec3 vertPos;
    vec3 vertNormal;
    vec3 vertTangent;
}
outp;

void main()
{
    gl_Position = vec4(0.0f);
    outp.vertPos = vec3(v_model * vec4(v_pos, 1.0f));
    outp.vertNormal = normalize(v_normalMatrix * v_normal);
    outp.vertTangent = normalize(v_normalMatrix * v_tangent);
}