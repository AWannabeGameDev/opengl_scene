#version 460 core

struct DirectionalLight
{
	vec3 direction;
	vec3 diffuseColor;
	vec3 specularColor;
};

in vsOut
{
    vec2 texCoord;
    mat3 tbnMatrix;
    vec3 fragPos;
    vec3 fragPosForDirLight;
} 
inp;

layout(std140) uniform lights
{
    DirectionalLight u_dirLight;
};

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;
uniform sampler2D u_normal;

uniform float u_ambience;
uniform float u_shininess;
uniform vec3 u_viewPosition;

uniform sampler2D u_dirLightShadowMap;

out vec4 fragColor;

vec3 normal = inp.tbnMatrix * texture(u_normal, inp.texCoord).rgb;

float shadowSampleBlurScale = 0.003f;
float depthBias = 0.01f * (1.0f - dot(normal, -u_dirLight.direction));

vec3 diffuseColor = texture(u_diffuse, inp.texCoord).rgb;
vec3 specularColor = texture(u_specular, inp.texCoord).rgb;

vec3 calcLighting(DirectionalLight dirLight)
{
    vec3 diffuseLighting = diffuseColor * dirLight.diffuseColor * clamp(dot(-dirLight.direction, normal), 0.0f, 1.0f);

    vec3 halfwayVector = normalize(-dirLight.direction + normalize(u_viewPosition - inp.fragPos));
    float specularStrength = pow(clamp(dot(normal, halfwayVector), 0.0f, 1.0f), u_shininess);
    vec3 specularLighting = specularColor * dirLight.specularColor * specularStrength;

    return diffuseLighting + specularLighting;
}

void main()
{
    fragColor = vec4(0.0f);

    fragColor.rgb += diffuseColor * u_ambience;

    float shadowStrength = 1.0f;
    
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            vec2 shadowSampleTexCoord = (inp.fragPosForDirLight.xy * 0.5f) + 0.5f + (shadowSampleBlurScale * vec2(i, j));

            float nearestDirLightDepth = texture(u_dirLightShadowMap, shadowSampleTexCoord).r;
            float fragDepthForDirLight = (inp.fragPosForDirLight.z * 0.5f) + 0.5f - depthBias;
            
            if(fragDepthForDirLight <= nearestDirLightDepth)
            {
                shadowStrength -= 1.0f / 9.0f;
            }
        }
    }

    fragColor.rgb += (1.0f - shadowStrength) * calcLighting(u_dirLight);
}