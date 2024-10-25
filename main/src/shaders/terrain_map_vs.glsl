#version 460 core

layout(location = 0) in vec3 v_pos;

uniform mat4 u_orthoBoxMatrix;

out vec3 color;

void main()
{
    gl_Position = u_orthoBoxMatrix * vec4(v_pos, 1.0f);
    color = (gl_Position.zzz * 0.5f) + 0.5f;
}