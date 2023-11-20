#ifndef FASTCG_LIGHTING_GLSL
#define FASTCG_LIGHTING_GLSL

#include "Fog.glsl"
#include "pcss/PCSS.glsl"
#include "ssao/SSAO.glsl"

layout(std140, BINDING_0_4) uniform LightingConstants 
{
	vec4 uLight0Position;
	vec4 uLight0ViewPosition;
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
    // FIXME: divergence control
	if (GetLightType() == 1.0) 
	{
	    float d = distance(uLight0Position.xyz, worldPosition);
		return 1.0 / max(uLight0ConstantAttenuation + uLight0LinearAttenuation * d + uLight0QuadraticAttenuation * pow(d, 2.0), 1.0);
	}
	else
	{
		return 1.0;
	}
}

vec4 Phong(vec4 diffuse, vec3 lightDirection, float viewDistance, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewDistance, fragColor);
    return fragColor;
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, float viewDistance, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = max(pow(max(dot(reflectionDirection, viewerDirection), 0.0), shininess), 0.0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewDistance, fragColor);
    return fragColor;
}

vec4 BlinnPhong(vec4 diffuse, vec3 lightDirection, float viewDistance, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewDistance, fragColor);
    return fragColor;
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, float viewDistance, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 halfwayVector = normalize(lightDirection + viewerDirection);
    float specularAttenuation = max(pow(max(dot(halfwayVector, normal), 0.0), shininess), 0.0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    vec4 fragColor = DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(uPCSSData, uShadowMap, worldPosition) * GetAmbientOcclusion(uAmbientOcclusionMap, screenCoords);
    fragColor = ApplyFog(viewDistance, fragColor);
    return fragColor;
}

#define Lighting BlinnPhong

#endif