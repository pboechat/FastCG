#ifndef FASTCG_LIGHTING_CONSTANTS_GLSL
#define FASTCG_LIGHTING_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(BINDING(0, 2)) uniform LightingConstants 
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
	mat4 uShadowMapViewProjection;
	float uShadowMapBias;
};

layout(BINDING(0, 3)) uniform sampler2D uShadowMap;
layout(BINDING(0, 4)) uniform sampler2D uAmbientOcclusionMap;

#define FASTCG_LIGHT_TYPE() uLight0Position.w

float GetShadow(vec3 worldPosition)
{
	vec4 projectedCoords = uShadowMapViewProjection * vec4(worldPosition, 1);

	vec3 shadowMapCoords = projectedCoords.xyz / projectedCoords.w;
	shadowMapCoords = shadowMapCoords * 0.5 + 0.5;

	float z = texture(uShadowMap, shadowMapCoords.xy).x;
	return float(z > (shadowMapCoords.z - uShadowMapBias));
}

float GetAmbientOcclusion(vec2 screenCoords)
{
	return min(texture(uAmbientOcclusionMap, screenCoords).x, 1.0);
}

#endif