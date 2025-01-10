#version 460 core

vec2 vertexPositions[6] =
{
    vec2(-1.0f, -1.0f), vec2(1.0f, -1.0f), vec2(-1.0f, 1.0f),
    vec2(-1.0f, 1.0f), vec2(1.0f, -1.0f), vec2(1.0f, 1.0f)
};

out vec2 texCoord;

void main()
{
    texCoord = (vertexPositions[gl_VertexID] * 0.5f) + 0.5f;
    gl_Position = vec4(vertexPositions[gl_VertexID], 1.0f, 1.0f);
}