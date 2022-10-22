#ifndef FASTCG_DEFERRED_MATERIAL_CONSTANTS_GLSL
#define FASTCG_DEFERRED_MATERIAL_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(BINDING(1, 0)) uniform MaterialConstants
{
	vec4 uDiffuseColor;
	vec4 uSpecularColor;
	vec2 uColorMapTiling;
	vec2 uBumpMapTiling;
	float uShininess;
};
layout(BINDING(0, 0)) uniform sampler2D uColorMap;
layout(BINDING(0, 1)) uniform sampler2D uBumpMap;

#endif