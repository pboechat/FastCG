#ifndef FASTCG_SHADOW_GLSL
#define FASTCG_SHADOW_GLSL

struct ShadowMapData
{
    mat4 viewProjection;
	float bias;
};

vec3 GetShadowMapCoordinates(ShadowMapData shadowMapData, vec3 worldPosition)
{
	vec4 clipPos = shadowMapData.viewProjection * vec4(worldPosition, 1);
	vec3 ndc = clipPos.xyz / clipPos.w;
#ifdef VULKAN
	return vec3(ndc.x * 0.5 + 0.5, 
				ndc.y * -0.5 + 0.5, // flip y
				ndc.z); // z already in [0, 1]
#else
	return ndc * 0.5 + 0.5; // z from [-1, 1] to [0, 1]
#endif
}

// for testing
float GetHardShadow(ShadowMapData shadowMapData, sampler2D shadowMap, vec3 worldPosition)
{
	vec3 shadowMapCoords = GetShadowMapCoordinates(shadowMapData, worldPosition);
	float z = texture(shadowMap, shadowMapCoords.xy).x;
	return float(z > (shadowMapCoords.z - shadowMapData.bias));
}

#define GetShadow GetHardShadow

#endif