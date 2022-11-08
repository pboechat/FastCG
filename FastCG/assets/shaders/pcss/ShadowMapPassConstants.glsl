#ifndef FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL
#define FASTCG_PCSS_SHADOW_MAP_PASS_CONSTANTS_GLSL

#include "../FastCG.glsl"

layout(BINDING(0, 0)) uniform SSAOHighFrequencyPassConstants
{
	mat4 uModelViewProjection;
};

#endif