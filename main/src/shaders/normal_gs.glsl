#version 460 core

layout(points) in;
layout(line_strip, max_vertices = 3) out;

in vsOut
{
    vec3 vertPos;
    vec3 vertNormal;
    vec3 vertTangent;
}
inp[];

layout(std140) uniform cameraMatrices
{
    mat4 u_view;
    mat4 u_proj;
};

void main()
{
    gl_Position = u_proj * u_view * vec4(inp[0].vertPos + (1.0f * inp[0].vertNormal), 1.0f);
    EmitVertex();

    gl_Position = u_proj * u_view * vec4(inp[0].vertPos, 1.0f);
    EmitVertex();

    gl_Position = u_proj * u_view * vec4(inp[0].vertPos + (1.0f * inp[0].vertTangent), 1.0f);
    EmitVertex();

    EndPrimitive();
}