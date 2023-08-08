#ifndef FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL
#define FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL

#include "../FastCG.glsl"

struct ShadowMapPassInstanceData
{
	mat4 modelViewProjection;
};

layout(BINDING_0_0) uniform ShadowMapPassConstants
{
	ShadowMapPassInstanceData uInstanceData[MAX_INSTANCES];
};

#define GetShadowMapPassInstanceData() uInstanceData[GetInstanceIndex()]

#endif