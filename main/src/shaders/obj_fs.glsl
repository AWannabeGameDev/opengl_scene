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
    vec3 fragPosForDirLight;
} 
inp;

layout(std140) uniform lights
{
    DirectionalLight u_dirLight;
};

uniform float u_ambience;
uniform float u_shininess;
uniform vec3 u_viewPosition;

uniform sampler2D u_dirLightShadowMap;

vec3 calcLighting(DirectionalLight dirLight)
{
    vec3 diffuseColor = inp.color * dirLight.diffuseColor * clamp(dot(-dirLight.direction, inp.normal), 0.0f, 1.0f);

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

    float depthBias = 0.001f * (1.0f - dot(inp.normal, -u_dirLight.direction));
    float nearestDirLightDepth = texture(u_dirLightShadowMap, (inp.fragPosForDirLight.xy * 0.5f) + 0.5f).r;
    float fragDepthForDirLight = (inp.fragPosForDirLight.z * 0.5f) + 0.5f - depthBias;

    float shadowStrength = 1.0f;
    if(fragDepthForDirLight <= nearestDirLightDepth)
    {
        shadowStrength = 0.0f;
    }

    fragColor.rgb += (1.0f - shadowStrength) * calcLighting(u_dirLight);
}