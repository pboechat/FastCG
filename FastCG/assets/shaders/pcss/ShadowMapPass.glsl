#ifndef FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL
#define FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL

#include "../FastCG.glsl"

struct ShadowMapPassInstanceaData
{
	mat4 modelViewProjection;
};

layout(BINDING_0_0) buffer ShadowMapPassConstants
{
	ShadowMapPassInstanceaData uInstanceData[];
};

#define GetShadowMapPassInstanceData() uInstanceData[GetInstanceIndex()]

#endif