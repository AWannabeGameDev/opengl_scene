#version 460 core

struct DirectionalLight
{
	vec3 direction;
	vec3 diffuseColor;
	vec3 specularColor;
};

in vsOut
{
    vec3 color;
    vec3 normal;
    vec3 fragPos;
} 
inp;

layout(std140) uniform lights
{
    DirectionalLight u_dirLight;
};

uniform float u_ambience;
uniform float u_shininess;
uniform vec3 u_viewPosition;

vec3 calcLighting(DirectionalLight dirLight)
{
    vec3 diffuseColor = inp.color * dirLight.diffuseColor *  dot(-dirLight.direction, inp.normal);

    vec3 halfwayVector = normalize(-dirLight.direction + normalize(u_viewPosition - inp.fragPos));
    float specularStrength = pow(clamp(dot(inp.normal, halfwayVector), 0.0f, 1.0f), u_shininess);
    vec3 specularColor = inp.color * dirLight.specularColor * specularStrength;

    return diffuseColor + specularColor;
}

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.0f);

    fragColor.rgb += inp.color * u_ambience;
    fragColor.rgb += calcLighting(u_dirLight);
}