#ifndef FASTCG_MATERIAL_CONSTANTS_GLSL
#define FASTCG_MATERIAL_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(BINDING_1_0) uniform MaterialConstants
{
	vec4 uDiffuseColor;
    vec4 uSpecularColor;
	vec2 uColorMapTiling;
	vec2 uBumpMapTiling;
	float uShininess;
};
layout(BINDING_1_1) uniform sampler2D uColorMap;
layout(BINDING_1_2) uniform sampler2D uBumpMap;

#endif