#version 460 core

in vec2 texCoord;

uniform float u_gamma;
uniform sampler2D u_colorTexture;

out vec4 fragColor;

void main()
{
    fragColor.rgb = pow(texture(u_colorTexture, texCoord).rgb, vec3(1 / u_gamma));
}