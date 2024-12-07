#ifndef FASTCG_LIGHTING_GLSL
#define FASTCG_LIGHTING_GLSL

#include "Fog.glsl"
#include "pcss/PCSS.glsl"
#include "ssao/SSAO.glsl"

layout(std140, BINDING_0_4) uniform LightingConstants 
{
	vec4 uLight0Position;
	vec4 uLight0DiffuseColor;
	vec4 uLight0SpecularColor;
	float uLight0Intensity;
	float uLight0ConstantAttenuation;
	float uLight0LinearAttenuation;
	float uLight0QuadraticAttenuation;
	vec4 uAmbientColor;
};

layout(BINDING_0_5) uniform sampler2D uShadowMap;
layout(BINDING_0_6) uniform sampler2D uAmbientOcclusionMap;

// returns 1 if point light and -1 if directional light
#define GetLightType() uLight0Position.w

float DistanceAttenuation(vec3 worldPosition)
{
    float D = distance(uLight0Position.xyz, worldPosition);
    float distanceAttenuation = 1.0 / max(uLight0ConstantAttenuation + uLight0LinearAttenuation * D + uLight0QuadraticAttenuation * D * D, 1e-8);
    return step(0.0, GetLightType()) * distanceAttenuation + step(GetLightType(), 0.0);
}

vec4 Phong(vec4 diffuse, vec3 lightDirection, float viewerDistance, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(0.0, dot(normal, lightDirection));
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewerDistance, fragColor);
    return fragColor;
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, float viewerDistance, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(0.0, dot(normal, lightDirection));
    vec4 diffuseContribution = uLight0DiffuseColor * diffuse * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = pow(max(0.0, dot(viewerDirection, reflectionDirection)), shininess);
    vec4 specularContribution = uLight0SpecularColor * specular * specularAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * uLight0Intensity * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewerDistance, fragColor);
    return fragColor;
}

vec4 BlinnPhong(vec4 diffuse, vec3 lightDirection, float viewerDistance, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(0.0, dot(normal, lightDirection));
    vec4 diffuseContribution = uLight0DiffuseColor * diffuse * diffuseAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * uLight0Intensity * (uAmbientColor + diffuseContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewerDistance, fragColor);
    return fragColor;
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, float viewerDistance, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(0.0, dot(normal, lightDirection));
    vec4 diffuseContribution = uLight0DiffuseColor * diffuse * diffuseAttenuation;

    vec3 halfwayVector = normalize(lightDirection + viewerDirection);
    float specularAttenuation = pow(max(0.0, dot(normal, halfwayVector)), shininess);
    vec4 specularContribution = uLight0SpecularColor * specular * specularAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * uLight0Intensity * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewerDistance, fragColor);
    return fragColor;
}

#define Lighting BlinnPhong

#endif