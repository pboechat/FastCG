#ifndef FASTCG_DEFERRED_LIGHTING_CONSTANTS_GLSL
#define FASTCG_DEFERRED_LIGHTING_CONSTANTS_GLSL

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
layout(BINDING(0, 2)) uniform sampler2D uDiffuseMap;
layout(BINDING(0, 3)) uniform sampler2D uNormalMap;
layout(BINDING(0, 4)) uniform sampler2D uSpecularMap;
layout(BINDING(0, 5)) uniform sampler2D uTangentMap;
layout(BINDING(0, 6)) uniform sampler2D uExtraData;
layout(BINDING(0, 7)) uniform sampler2D uDepth;
layout(BINDING(0, 8)) uniform sampler2D uAmbientOcclusionMap;

#define FASTCG_LIGHT_TYPE() uLight0Position.w

#endif