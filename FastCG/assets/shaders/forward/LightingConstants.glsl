#ifndef FASTCG_FORWARD_LIGHTING_CONSTANTS_GLSL
#define FASTCG_FORWARD_LIGHTING_CONSTANTS_GLSL

#include "FastCG.glsl"

layout(BINDING(0, 2)) uniform LightingConstants 
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

#define FASTCG_LIGHT_TYPE() uLight0Position.w

#endif