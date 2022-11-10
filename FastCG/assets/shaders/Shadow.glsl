#ifndef FASTCG_SHADOW_GLSL
#define FASTCG_SHADOW_GLSL

struct ShadowMapData
{
    mat4 viewProjection;
	float bias;
};

vec3 GetShadowMapCoordinates(ShadowMapData shadowMapData, vec3 worldPosition)
{
	vec4 clipCoords = shadowMapData.viewProjection * vec4(worldPosition, 1);
	vec3 shadowMapCoords = clipCoords.xyz / clipCoords.w;
	return shadowMapCoords * 0.5 + 0.5;
}

// mostly for testing
float GetHardShadow(ShadowMapData shadowMapData, sampler2D shadowMap, vec3 worldPosition)
{
	vec3 shadowMapCoords = GetShadowMapCoordinates(shadowMapData, worldPosition);
	float z = texture(shadowMap, shadowMapCoords.xy).x;
	return float(z > (shadowMapCoords.z - shadowMapData.bias));
}

#define GetShadow GetHardShadow

#endif